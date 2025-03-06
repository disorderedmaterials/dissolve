#include "nodes/graph.h"

// Add nodes
void Graph::addNode(std::unique_ptr<Node> node, std::string_view name) { nodes_.insert(std::make_pair(name, node)); }

// Add parameter link between nodes
void Graph::addEdge(Node::Edge &linkMap) { edges_.push_back(linkMap); }

// Return container of nodes
Graph::Nodes &Graph::nodes() { return nodes_; }

// Return container of parameter links between nodes
Graph::Edges &Graph::edges() { return edges_; }
