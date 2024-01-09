// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Expression Unary Operator Node
class ExpressionUnaryOperatorNode : public ExpressionNode
{
    public:
    // Unary Operators Enum
    enum UnaryOperator
    {
        OperatorNegate
    };
    ExpressionUnaryOperatorNode(UnaryOperator op);
    ~ExpressionUnaryOperatorNode() override = default;

    /*
     * Nodes
     */
    protected:
    // Duplicate this node and its contents
    std::shared_ptr<ExpressionNode> duplicate() override;

    /*
     * Data
     */
    private:
    // Operator type
    UnaryOperator operator_;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    std::optional<ExpressionValue> evaluate() const override;
    // Return string representation of node
    std::string asString() const override;
};
