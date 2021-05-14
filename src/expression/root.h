// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Expression Root Node
class ExpressionRootNode : public ExpressionNode
{
    public:
    ExpressionRootNode();
    ~ExpressionRootNode() override = default;

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
};
