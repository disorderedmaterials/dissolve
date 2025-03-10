// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
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
    explicit Graph(std::unique_ptr<Node> &&parent) : Node(this), parent_(std::move(parent)) {}
    ~Graph() = default;

    /*
     * Nodes and edges
     */
    using Nodes = std::map<std::string_view, std::unique_ptr<Node>>;
    using Edge = Node::LinkMap;
    using Edges = std::vector<Edge>;

    private:
    // Parent node
    std::unique_ptr<Node> parent_;
    // Container of nodes
    Nodes nodes_{};
    // Container of parameter links between nodes
    Edges edges_{};

    public:
    // Produce a registered node by type
    static std::unique_ptr<Node> produceNode(const std::string_view& nodeType) { return registry.find(nodeType)->second(); }
    // Add node
    void addNode(std::unique_ptr<Node> &&node, std::string_view name);
    // Add parameter link between nodes
    void addEdge(Edge &linkMap);
    // Return container of nodes
    Nodes &nodes();
    // Return container of parameter links between nodes
    Edges &edges();
};
