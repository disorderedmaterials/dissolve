// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"

// Add Node
class AddNode : public Node
{
    public:
    AddNode();
    ~AddNode() override = default;

    /*
     * Definition
     */
    public:
    // Return short name of the node
    std::string_view name() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Processing & Validity
     */
    private:
    // Operand A
    Number a_;
    // Operand B
    Number b_;
    // Result (sum of A and B)
    Number result_;

    public:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
