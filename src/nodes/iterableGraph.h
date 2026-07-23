// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/edge.h"
#include "nodes/graph.h"
#include "nodes/inputs.h"
#include "nodes/loopBack.h"
#include "templates/doubleKeyedMap.h"

class IterableGraph : public Graph
{
    public:
    IterableGraph(Graph *parentGraph);
    ~IterableGraph() = default;

    using LoopEdges = std::vector<std::unique_ptr<LoopEdge>>;

    /*
     * Definition
     */
    public:
    // Return node name
    std::string_view name() const override;
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Number of loops (iterations) to perform
    Number nIterations_{0};
    // Current loop iteration
    int i_{1};
    // Loop backs
    LoopBacksNode *loopBacks_{nullptr};
    // Loop edges
    Edges loopEdges_;
    // Set the loopbacks corresponding to the graph inputs
    void setLoopBacks();
    // Release a loopback by name
    void releaseLoopBack(const std::string &name);

    private:
    // Add edge between nodes
    bool addLoopEdge(std::unique_ptr<Edge> edge, std::string_view source);
    // Add edge to node map
    Edge *addOutputLoopEdge(std::string_view sourceOutput, Edge *edge);
    // Remove edge from node map
    Edge *removeOutputLoopEdge(std::string_view sourceOutput, Edge *edge);

    public:
    // Current loop iteration
    int currentIteration() const;
    // Number of loops (iterations) to perform
    int nIterations() const;
    // Return loopbacks node
    LoopBacksNode *loopBacks() const;
    // Return loop edges
    const Edges &loopEdges() const;
    // Remove specified loop edge
    bool removeEdge(LoopEdge *edgeToRemove);
    // Find loop edge between nodes
    LoopEdge *findLoopEdge(const EdgeDefinition &definition) const;

    /*
     * Nodes and Edges
     */
    public:
    // Add edge between nodes
    bool addEdge(const EdgeDefinition &definition) override;
    // Remove edge between nodes
    bool removeEdge(const EdgeDefinition &definition) override;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
