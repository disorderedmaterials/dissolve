// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"

// MultiplyNode Node
class MultiplyNode : public Node
{
    public:
    MultiplyNode();
    ~MultiplyNode() override = default;

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Factor A
    Number a_;
    // Factor B
    Number b_;
    // Product of A and B
    Number result_;

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
