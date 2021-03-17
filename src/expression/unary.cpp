// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "expression/unary.h"

ExpressionUnaryOperatorNode::ExpressionUnaryOperatorNode(UnaryOperator op) : ExpressionNode(), operator_(op) {}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionUnaryOperatorNode::duplicate()
{
    auto node = std::make_shared<ExpressionUnaryOperatorNode>(operator_);

    for (auto child : children_)
        node->addChild(child->duplicate());

    return node;
}

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
        default:
            throw(std::runtime_error(fmt::format("ExpressionUnaryOperatorNode - unhandled operator {}.\n", operator_)));
    }

    return result;
}
