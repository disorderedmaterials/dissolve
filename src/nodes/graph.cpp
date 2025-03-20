#include "nodes/graph.h"

// Add nodes
void Graph::addNode(std::unique_ptr<Node> &&node, std::string_view name)
{
    node->setParentGraph(this);
    nodes_.insert(std::make_pair(name, std::move(node)));
}

// Add parameter link between nodes
bool Graph::addEdge(Graph::Edge edge)
{
    auto source = nodes_[edge.source]->findParameter(edge.sourceParam);
    auto &sink = nodes_[edge.edge];
    return sink->link(edge.edgeParam, *source);
}

// Return container of nodes
Graph::Nodes &Graph::nodes() { return nodes_; }

// Return container of parameter links between nodes
Graph::Edges Graph::edges()
{
    Edges edges;
    for (auto &[sink, node] : nodes_)
    {
        for (auto &[unused, link] : node->links())
        {
            auto source = link.source().parent();
            auto entry = std::find_if(nodes_.begin(), nodes_.end(),
                                      [source](const auto &it)
                                      {
                                          auto &[key, value] = it;
                                          return value.get() == source;
                                      });

            // For some reason, clang can't deduce the correct
            // constructor if I pass everything into emplace_back, so
            // I have to make this wrapper.
            Graph::Edge newedge = {entry->first, link.source().name(), sink, link.sink().name()};
            edges.emplace_back(newedge);
        }
    }
    return edges;
}

// Return short name of the node
std::string_view Graph::name() const { return "Graph"; }

// Return short summary of the node's purpose
std::string_view Graph::summary() const { return "A node which contains its own inner graph"; }

// Express as a serialisable value
SerialisedValue Graph::serialise() const
{
    SerialisedValue graph, result = Node::serialise();
    for (auto &[k, v] : nodes_)
        graph[std::string(k)] = *v;
    result["graph"] = graph;
    return result;
}

// Read values from a serialisable value
void Graph::deserialise(const SerialisedValue &node)
{
    Node::deserialise(node);
    toMap(node, "graph",
          [this](const auto name, const auto &value)
          {
              std::string kind = toml::find<std::string>(value, "name");
              if (!registry.contains(kind))
                  Messenger::exception("Attempted to create node of unknown kind: {}", kind);
              auto child = registry.at(kind)();
              child->deserialise(value);
              addNode(std::move(child), name);
          });
}
