// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ~ExpressionBinaryOperatorNode() = default;

    /*
     * Nodes
     */
    protected:
    // Duplicate this node and its contents
    std::shared_ptr<ExpressionNode> duplicate();

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
    virtual std::optional<ExpressionValue> evaluate() const;
};
