// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Expression Binary Operator Node
class ExpressionBinaryOperatorNode : public ExpressionNode
{
    public:
    // Binary Operators Enum
    enum BinaryOperator
    {
        OperatorAdd,
        OperatorDivide,
        OperatorMultiply,
        OperatorPow,
        OperatorSubtract,
        OperatorLessEqual,
        OperatorGreaterEqual,
        OperatorEqual,
        OperatorLessThan,
        OperatorGreaterThan,
    };
    ExpressionBinaryOperatorNode(BinaryOperator op);
    ~ExpressionBinaryOperatorNode() override = default;

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
    BinaryOperator operator_;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    std::optional<ExpressionValue> evaluate() const override;
    // Return string representation of node
    std::string asString() const override;
};
