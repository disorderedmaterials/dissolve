// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Expression Root Node
class ExpressionRootNode : public ExpressionNode
{
    public:
    explicit ExpressionRootNode(bool parenthesesEnclosed = false);
    ~ExpressionRootNode() override = default;

    /*
     * Data
     */
    private:
    // Whether parentheses surround the nodes
    bool parenthesesEnclosed_;

    /*
     * Nodes
     */
    public:
    // Duplicate this node and its contents
    std::shared_ptr<ExpressionNode> duplicate() override;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    std::optional<ExpressionValue> evaluate() const override;
    // Return string representation of node
    std::string asString() const override;
};
