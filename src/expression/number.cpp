// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/number.h"

ExpressionNumberNode::ExpressionNumberNode(int i) : ExpressionNode() { value_ = i; }

ExpressionNumberNode::ExpressionNumberNode(double d) : ExpressionNode() { value_ = d; }

ExpressionNumberNode::ExpressionNumberNode(const ExpressionValue &value) : ExpressionNode() { value_ = value; }

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionNumberNode::duplicate() { return std::make_shared<ExpressionNumberNode>(value_); }

/*
 * Evaluation
 */

// Evaluate node
std::optional<ExpressionValue> ExpressionNumberNode::evaluate() const
{
    // Must be zero children
    if (children_.size() != 0)
        return std::nullopt;

    return value_;
}

// Return string representation of node
std::string ExpressionNumberNode::asString() const { return value_.asString(); }
