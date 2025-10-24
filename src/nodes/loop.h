// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/graph.h"

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
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Whether changes to source parameters of loopback edges invalidates our status
    bool loopbackInvalidates_{true};

    public:
    // Number of iterations to loop for
    Number iterations_{1};

    /*
     * Processing & Validity
     */
    public:
    // Perform processing
    NodeConstants::ProcessResult process();
};
