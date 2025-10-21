// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/edge.h"
#include "nodes/graph.h"
#include "nodes/inputs.h"
#include "nodes/outputs.h"
#include "templates/doubleKeyedMap.h"

// Loop Graph
class LoopGraph : public Graph
{
    public:
    LoopGraph(Graph *parentGraph);
    ~LoopGraph() = default;

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
    // Increment loop counter
    void operator++() { loopCounter_++; }

    /*
     * Feedback
     *
     */
    private:
    // Number of loops (iterations) to perform
    int nLoops_{0};
    // Current loop
    int loopCounter_{0};
    // LoopBacks
    OutputsNode *loopBacks_{nullptr};
    // Release a loopback by name
    void releaseLoopBack(const std::string &name);

    public:
    // Unlink edge, releasing the loop back if one accompanies it
    void unlinkEdge(Edge *edge) override;
    // Reset the loop counter to zero
    void resetLoopCounter();
    // Set the loopbacks corresponding to the graph inputs
    void setLoopBacks();

    /*
     * Processing & Validity
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    /*
     * Functions
     */
    public:
};
