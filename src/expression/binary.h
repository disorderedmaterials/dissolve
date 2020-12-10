// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/nodeNEW.h"

// Expression Binary Operator Node
class ExpressionBinaryOperatorNode : public ExpressionNodeNEW
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
