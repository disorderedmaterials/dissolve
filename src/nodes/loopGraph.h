// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/edge.h"
#include "nodes/graph.h"
#include "nodes/inputs.h"
#include "nodes/loopBack.h"
#include "nodes/outputs.h"
#include "templates/doubleKeyedMap.h"

// Loop Graph
class LoopGraph : public Graph
{
    public:
    LoopGraph(Graph *parentGraph);
    ~LoopGraph() = default;

    using LoopEdges = std::vector<std::unique_ptr<LoopEdge>>;

    /*
     * Definition (Virtuals)
     */
    public:
    // Return node name
    std::string_view name() const override;
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Feedback
     */
    private:
    // Number of loops (iterations) to perform
    Number nIterations_{0};
    // Current loop iteration
    int i_{0};
    // Loop backs
    LoopBacksNode *loopBacks_{nullptr};
    // Loop edges
    Edges loopEdges_;
    // Increment loop counter
    void increment();
    // Current loop iteration
    int loopCount();
    // Reset the loop counter to zero
    void reset();
    // Set the loopbacks corresponding to the graph inputs
    void setLoopBacks();
    // Release a loopback by name
    void releaseLoopBack(const std::string &name);

    public:
    // Number of loops (iterations) to perform
    const int nLoops() const;
    // Loop backs
    LoopBacksNode *loopBacks();
    // Loop edges
    Edges &loopEdges();
    // Add edge between nodes
    bool addEdge(const EdgeDefinition &definition) override;
    // Remove edge between nodes
    bool removeEdge(const EdgeDefinition &definition) override;
    bool removeEdge(LoopEdge *edgeToRemove);
    // Find loop edge between nodes
    LoopEdge *findLoopEdge(const EdgeDefinition &definition) const;

    /*
     * Processing & Validity
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
