// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/unary.h"

ExpressionUnaryOperatorNode::ExpressionUnaryOperatorNode(UnaryOperator op) : ExpressionNode(), operator_(op) {}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionUnaryOperatorNode::duplicate()
{
    auto node = std::make_shared<ExpressionUnaryOperatorNode>(operator_);

    for (const auto &child : children_)
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

    // Evaluate RHS node
    auto optr = children_[0]->evaluate();
    if (!optr)
        return std::nullopt;

    // Perform the operation
    ExpressionValue result;
    auto rhs = (*optr);
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

// Return string representation of node
std::string ExpressionUnaryOperatorNode::asString() const
{
    // Must be a single child node
    if (children_.size() != 1)
        return "";

    // Evaluate RHS node
    auto rhs = children_[0]->asString();

    switch (operator_)
    {
        case (OperatorNegate):
            return fmt::format("-{}", rhs);
        default:
            throw(std::runtime_error(fmt::format("ExpressionUnaryOperatorNode - unhandled operator {}.\n", operator_)));
    }

    return "";
}
