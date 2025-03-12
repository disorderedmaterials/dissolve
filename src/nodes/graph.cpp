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
    auto source = nodes_[edge.source]->findInput(edge.sourceParam);
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
            edges.emplace_back(entry->first, link.source().name(), sink, link.sink().name());
        }
    }
    return edges;
}

// Return short name of the node
std::string_view Graph::name() { return "Graph"; }

// Return short summary of the node's purpose
std::string_view Graph::summary() { return "A node which contains its own inner graph"; }
