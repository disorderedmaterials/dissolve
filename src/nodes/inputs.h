// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"

// Inputs Node
class InputsNode : public Node
{
    public:
    InputsNode(Graph *parentGraph, NodeParameterMap &inputs);
    ~InputsNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Inputs
     */
    private:
    // Inputs to manage
    NodeParameterMap &inputs_;

    public:
    // Add input
};
