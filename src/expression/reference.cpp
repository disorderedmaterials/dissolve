// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/reference.h"
#include "expression/variable.h"
#include <utility>

ExpressionReferenceNode::ExpressionReferenceNode(std::shared_ptr<ExpressionVariable> variable)
    : ExpressionNode(), variable_(std::move(variable))
{
}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionReferenceNode::duplicate()
{
    return std::make_shared<ExpressionReferenceNode>(variable_);
}

/*
 * Evaluation
 */

// Evaluate node
std::optional<ExpressionValue> ExpressionReferenceNode::evaluate() const
{
    assert(variable_);

    return (variable_->value());
}

// Return string representation of node
std::string ExpressionReferenceNode::asString() const
{
    assert(variable_);

    return std::string(variable_->name());
}
