// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"

// Add Node
class AddNode : public Node
{
    public:
    AddNode(Graph *parentGraph);
    ~AddNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Processing & Validity
     */
    private:
    // Operand X
    Number x_;
    // Operand Y
    Number y_;
    // Result (sum of X and Y)
    Number result_;

    public:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
