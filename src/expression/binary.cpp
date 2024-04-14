// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/binary.h"
#include "math/mathFunc.h"

ExpressionBinaryOperatorNode::ExpressionBinaryOperatorNode(BinaryOperator op) : ExpressionNode(), operator_(op) {}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionBinaryOperatorNode::duplicate()
{
    auto node = std::make_shared<ExpressionBinaryOperatorNode>(operator_);

    for (const auto &child : children_)
        node->addChild(child->duplicate());

    return node;
}

/*
 * Evaluation
 */

// Evaluate node
std::optional<ExpressionValue> ExpressionBinaryOperatorNode::evaluate() const
{
    // Must be two child nodes
    if (children_.size() != 2)
        return std::nullopt;

    // Evaluate LHS and RHS nodes
    auto optl = children_[0]->evaluate();
    if (!optl)
        return std::nullopt;
    auto optr = children_[1]->evaluate();
    if (!optr)
        return std::nullopt;

    // Perform the operation
    ExpressionValue result;
    auto lhs = (*optl), rhs = (*optr);
    switch (operator_)
    {
        case (OperatorAdd):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() + rhs.asInteger();
            else
                // TODO: do we need to consider the case where we have bools?
                result = lhs.asDouble() + rhs.asDouble();
            break;
        case (OperatorDivide):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() / rhs.asInteger();
            else
                result = lhs.asDouble() / rhs.asDouble();
            break;
        case (OperatorSubtract):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() - rhs.asInteger();
            else
                result = lhs.asDouble() - rhs.asDouble();
            break;
        case (OperatorPow):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = DissolveMath::power(lhs.asInteger(), rhs.asInteger());
            else
                result = pow(lhs.asDouble(), rhs.asDouble());
            break;
        case (OperatorMultiply):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() * rhs.asInteger();
            else
                result = lhs.asDouble() * rhs.asDouble();
            break;
        case (OperatorLessEqual):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() <= rhs.asInteger();
            else
                result = lhs.asDouble() <= rhs.asDouble();
            break;
        case (OperatorGreaterEqual):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() >= rhs.asInteger();
            else
                result = lhs.asDouble() >= rhs.asDouble();
            break;
        case (OperatorEqual):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() == rhs.asInteger();
            else
                result = lhs.asDouble() == rhs.asDouble();
            break;
        case (OperatorLessThan):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() < rhs.asInteger();
            else
                result = lhs.asDouble() < rhs.asDouble();
            break;
        case (OperatorGreaterThan):
            if (ExpressionValue::bothIntegers(lhs, rhs))
                result = lhs.asInteger() > rhs.asInteger();
            else
                result = lhs.asDouble() > rhs.asDouble();
            break;
        default:
            throw(std::runtime_error(fmt::format("ExpressionBinaryOperatorNode - unhandled operator {}.\n", operator_)));
    }

    return result;
}

// Return string representation of node
std::string ExpressionBinaryOperatorNode::asString() const
{
    // Must be two child nodes
    if (children_.size() != 2)
        return "";

    // Evaluate LHS and RHS nodes
    auto lhs = children_[0]->asString();
    auto rhs = children_[1]->asString();

    switch (operator_)
    {
        case (OperatorAdd):
            return fmt::format("{}+{}", lhs, rhs);
        case (OperatorDivide):
            return fmt::format("{}/{}", lhs, rhs);
        case (OperatorSubtract):
            return fmt::format("{}-{}", lhs, rhs);
        case (OperatorPow):
            return fmt::format("{}^{}", lhs, rhs);
        case (OperatorMultiply):
            return fmt::format("{}*{}", lhs, rhs);
        case (OperatorLessEqual):
            return fmt::format("{}<={}", lhs, rhs);
        case (OperatorGreaterEqual):
            return fmt::format("{}>={}", lhs, rhs);
        case (OperatorEqual):
            return fmt::format("{}=={}", lhs, rhs);
        case (OperatorLessThan):
            return fmt::format("{}<{}", lhs, rhs);
        case (OperatorGreaterThan):
            return fmt::format("{}>{}", lhs, rhs);
        default:
            throw(std::runtime_error(fmt::format("ExpressionBinaryOperatorNode - unhandled operator {}.\n", operator_)));
    }

    return "";
}
