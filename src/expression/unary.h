// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/nodeNEW.h"

// Expression Unary Operator Node
class ExpressionUnaryOperatorNode : public ExpressionNodeNEW
{
    public:
    // Unary Operators Enum
    enum UnaryOperator
    {
        OperatorNegate
    };
    ExpressionUnaryOperatorNode(UnaryOperator op);
    ~ExpressionUnaryOperatorNode() = default;

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
    virtual std::optional<ExpressionValue> evaluate() const;
};
