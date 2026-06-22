// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"

class MultiplyNode : public Node
{
    public:
    MultiplyNode(Graph *parentGraph);
    ~MultiplyNode() override = default;

    /*
     * Definition
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
    // Factor X
    Number x_;
    // Factor Y
    Number y_;
    // Product of X and Y
    Number result_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
