// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/unary.h"

ExpressionUnaryOperatorNode::ExpressionUnaryOperatorNode(UnaryOperator op) : ExpressionNodeNEW(), operator_(op) {}

/*
 * Evaluation
 */

// Evaluate node
std::optional<ExpressionValue> ExpressionUnaryOperatorNode::evaluate() const
{
    // Must be a single child node
    if (children_.size() != 1)
        return std::nullopt;

    // Evaluate LHS node
    auto optl = children_[0]->evaluate();
    if (!optl)
        return std::nullopt;

    // Perform the operation
    ExpressionValue result;
    auto rhs = (*optl);
    switch (operator_)
    {
        case (OperatorNegate):
            if (rhs.isInteger())
                result = -rhs.asInteger();
            else
                result = -rhs.asDouble();
            break;
    }

    return result;
}
