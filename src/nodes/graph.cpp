#include "nodes/graph.h"

// Add nodes
void Graph::addNode(std::unique_ptr<Node> &&node, std::string_view name)
{
    node->setParentGraph(this);
    node->setName(name);
    nodes_.insert(std::make_pair<std::string, std::unique_ptr<Node>>(std::string(name), std::move(node)));
}

// Add parameter link between nodes
bool Graph::addEdge(const EdgeDefinition &definition)
{
    auto edge = Edge::create(this, definition);
    if (!edge)
        return false;

    edges_.emplace_back(std::move(edge));

    return true;
}

// Remove edge between nodes
bool Graph::removeEdge(const EdgeDefinition &definition)
{
    auto edge = findEdge(definition);
    if (!edge)
        return Messenger::error("Edge doesn't exist, so can't remove it.\n");
    else
        return removeEdge(edge);
}
bool Graph::removeEdge(Edge *edgeToRemove)
{
    auto it =
        std::find_if(edges_.begin(), edges_.end(), [edgeToRemove](const auto &edge) { return edge.get() == edgeToRemove; });
    if (it == edges_.end())
        return Messenger::error("Edge pointer doesn't exist, so can't remove it.\n");
    edges_.erase(it);
    return true;
}

// Find edge between nodes
Edge *Graph::findEdge(const EdgeDefinition &definition) const
{
    auto it =
        std::find_if(edges_.begin(), edges_.end(), [definition](const auto &edge) { return edge->definition() == definition; });

    if (it != edges_.end())
        return it->get();

    return {};
}

// Return named node, if it exists
Node *Graph::node(std::string_view name)
{
    if (nodes_.contains(std::string(name)))
        return nodes_[std::string(name)].get();

    return nullptr;
}

// Return container of nodes
Graph::Nodes &Graph::nodes() { return nodes_; }

// Return edges on the graph
Graph::Edges &Graph::edges() { return edges_; }

// Return type of the node
std::string_view Graph::type() const { return "Graph"; }

// Return short summary of the node's purpose
std::string_view Graph::summary() const { return "A node which contains its own inner graph"; }

// Express as a serialisable value
SerialisedValue Graph::serialise() const
{
    SerialisedValue result = Node::serialise();
    fromMap(nodes_, "nodes", result);
    fromVector(edges_, "edges", result);
    return result;
}

// Read values from a serialisable value
void Graph::deserialise(const SerialisedValue &node)
{
    Node::deserialise(node);
    toMap(node, "nodes",
          [this](const auto name, const auto &value)
          {
              std::string kind = toml::find<std::string>(value, "type");
              if (!registry.contains(kind))
                  Messenger::exception("Attempted to create node of unknown kind: {}", kind);
              auto child = registry.at(kind)();
              child->deserialise(value);
              addNode(std::move(child), name);
          });
    toVector(node, "edges", [this](const auto &value) { addEdge(toml::get<EdgeDefinition>(value)); });
}
