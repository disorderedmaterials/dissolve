// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "nodes/node.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Forward Declarations
class Edge;
class EdgeDefinition;
class InputsNode;
class OutputsNode;

// Graph
class Graph : public Node
{
    public:
    Graph(Graph *parentGraph);
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
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Flag that the node data needs to be updated
    void setUpdateRequired() override;

    /*
     * Inputs, Outputs, and Options
     */
    private:
    // Proxy input and output nodes
    InputsNode *proxyInputs_{nullptr};
    OutputsNode *proxyOutputs_{nullptr};

    public:
    InputsNode &proxyInputs();
    OutputsNode &proxyOutputs();

    public:
    // Add supplied proxy input, setting ownership of the parameters appropriately
    bool addProxyInput(std::shared_ptr<ParameterBase> &input, std::shared_ptr<ParameterBase> &output);
    // Add supplied proxy output, setting ownership of the parameters appropriately
    bool addProxyOutput(std::shared_ptr<ParameterBase> &input, std::shared_ptr<ParameterBase> &output);

    /*
     * Nodes and Edges
     */
    public:
    // Typedefs for Node and Edge storage
    using Nodes = std::map<std::string, std::unique_ptr<Node>>;
    using ReverseNodes = std::map<const Node *, std::string>;
    using Edges = std::vector<std::unique_ptr<Edge>>;

    protected:
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
    // Create node of specified type with the name provided
    Node *createNode(std::string_view type, std::string_view name = {});
    // Add node to graph
    Node *addNode(std::unique_ptr<Node> node, std::string_view name = {});
    // Get name of specified child node
    std::string_view nodeName(const Node *node) const;
    // Set name of specified child node
    void setNodeName(const Node *node, std::string_view nodeName);
    // Add edge between nodes
    virtual bool addEdge(const EdgeDefinition &definition);
    // Remove edge between nodes
    virtual bool removeEdge(const EdgeDefinition &definition);
    bool removeEdge(Edge *edgeToRemove);
    // Find edge between nodes
    Edge *findEdge(const EdgeDefinition &definition) const;
    // Return named node, if it exists
    Node *findNode(std::string_view nodeName);
    // Return container of nodes
    Nodes &nodes();
    // Return container of reverse nodes
    ReverseNodes &reverseNodes();
    // Return container of edges between nodes
    Edges &edges();
    // Return a path to this graph from the root
    std::string location() const;
    // Print as mermaid state diagram
    std::string toMermaid(int depth) const;
    friend std::ostream &operator<<(std::ostream &stream, const Graph &node);

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
