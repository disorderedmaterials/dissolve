// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "nodes/edge.h"
#include "nodes/node.h"
#include "nodes/registry.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Graph
class Graph : public Node
{
    public:
    explicit Graph(Node *parent) : Node(this), parent_(parent) {}
    ~Graph() = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Nodes and Edges
     */
    public:
    // Typedefs for Node and Edge storage
    using Nodes = std::map<std::string, std::unique_ptr<Node>>;
    using ReverseNodes = std::map<const Node *, std::string>;
    using Edges = std::vector<std::unique_ptr<Edge>>;

    private:
    // Parent node
    Node *parent_;
    // Map of node names to nodes
    Nodes nodes_;
    // Map of nodes to node names
    ReverseNodes reverseNodes_;
    // Container of edges
    Edges edges_;

    private:
    // Get unique node name
    std::string uniqueNodeName(const Node *node, std::string_view baseName) const;

    public:
    // Produce a registered node by type
    static std::unique_ptr<Node> produceNode(const std::string_view &nodeType) { return registry.find(nodeType)->second(); }
    // Add node
    void addNode(std::unique_ptr<Node> &&node, std::string_view name);
    // Get name of specified child node
    std::string_view nodeName(const Node *node) const;
    // Set name of specified child node
    void setNodeName(const Node *node, std::string_view name);
    // Add edge between nodes
    bool addEdge(const EdgeDefinition &definition);
    // Remove edge between nodes
    bool removeEdge(const EdgeDefinition &definition);
    bool removeEdge(Edge *edgeToRemove);
    // Find edge between nodes
    Edge *findEdge(const EdgeDefinition &definition) const;
    // Return named node, if it exists
    Node *node(std::string_view name);
    // Return container of nodes
    Nodes &nodes();
    // Return container of edges between nodes
    Edges &edges();

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
