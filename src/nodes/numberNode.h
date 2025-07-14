// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"

// Number Node
class NumberNode : public Node
{
    public:
    NumberNode(Graph *parentGraph);
    ~NumberNode() override = default;

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // The number
    Number a_;

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
