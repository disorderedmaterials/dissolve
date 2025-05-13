// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/graph.h"
#include "nodes/inputs.h"

// SubGraph
class SubGraph : public Graph
{
    public:
    SubGraph(Graph *parentGraph);
    ~SubGraph() = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Permanent Nodes
     */
    private:
    InputsNode inputsNode_;
};
