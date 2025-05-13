// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/edge.h"
#include "nodes/registry.h"

Graph::Graph(Graph *parentGraph) : Node(parentGraph) {}

/*
 * Definition
 */

// Return type of the node
std::string_view Graph::type() const { return "Graph"; }

// Return short summary of the node's purpose
std::string_view Graph::summary() const { return "A node which contains its own inner graph"; }

/*
 * Nodes and Edges
 */

// Get unique node name
std::string Graph::uniqueNodeName(const Node *node, std::string_view baseName) const
{
    auto newName = std::string(baseName);

    // Check for existing node with this name and suffix until we get a unique key
    auto count = 1;
    while (nodes_.contains(newName) && nodes_.at(newName).get() != node)
        newName = std::format("{}{}", baseName, count++);

    return newName;
}

// Create node of specified type with the name provided
Node *Graph::createNode(std::string_view type, std::string_view name)
{
    return addNode(NodeRegistry::produce(this, type), name.empty() ? type : name);
}

// Add node to graph
Node *Graph::addNode(std::unique_ptr<Node> node, std::string_view newName)
{
    auto nodePtr = node.get();

    auto uniqueName = uniqueNodeName(node.get(), newName.empty() ? node->type() : newName);
    reverseNodes_.insert(std::make_pair<Node *, std::string>(node.get(), std::string(uniqueName)));
    nodes_.insert(std::make_pair<std::string, std::unique_ptr<Node>>(std::string(uniqueName), std::move(node)));

    return nodePtr;
}

// Get name of specified child node
std::string_view Graph::nodeName(const Node *node) const
{
    if (reverseNodes_.contains(node))
        return reverseNodes_.at(node);

    return "UNKNOWN_NODE";
}

// Set name of specified child node
void Graph::setNodeName(const Node *node, std::string_view name)
{
    auto uniqueName = uniqueNodeName(node, name);

    // Extract the forward node mapping (name -> node) using its current name in reverseNodes_
    auto nodeHandle = nodes_.extract(reverseNodes_.at(node));

    // Set the new name and reinsert
    nodeHandle.key() = uniqueName;
    nodes_.insert(std::move(nodeHandle));

    // Update reverseNodes_
    reverseNodes_[node] = uniqueName;
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

/*
 * I/O
 */

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
              std::string nodeType = toml::find<std::string>(value, "type");
              auto child = createNode(nodeType, name);

              child->deserialise(value);
          });
    toVector(node, "edges", [this](const auto &value) { addEdge(toml::get<EdgeDefinition>(value)); });
}
