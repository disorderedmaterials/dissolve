// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
        OperatorSubtract
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
};
