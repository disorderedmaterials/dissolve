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
    explicit Graph(Node *parent) : Node(this), parent_(parent) {}
    ~Graph() = default;

    /*
     * Nodes and edges
     */
    using Nodes = std::map<std::string_view, std::unique_ptr<Node>>;
    struct Edge
    {
        std::string_view source, sourceParam, edge, edgeParam;
        bool operator==(const Edge &other) const = default;
    };
    using Edges = std::vector<Edge>;

    private:
    // Parent node
    Node *parent_;
    // Container of nodes
    Nodes nodes_{};

    public:
    // Return short name of the node
    std::string_view name() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;
    // Produce a registered node by type
    static std::unique_ptr<Node> produceNode(const std::string_view &nodeType) { return registry.find(nodeType)->second(); }
    // Add node
    void addNode(std::unique_ptr<Node> &&node, std::string_view name);
    // Add parameter link between nodes
    bool addEdge(Edge edge);
    // Return container of nodes
    Nodes &nodes();
    // Return container of parameter links between nodes
    Edges edges();

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
