// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "expression/reference.h"
#include "expression/variable.h"

ExpressionReferenceNode::ExpressionReferenceNode(std::shared_ptr<ExpressionVariable> variable)
    : ExpressionNode(), variable_(variable)
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
    // Must have a valid pointer
    if (!variable_)
        return std::nullopt;

    return (variable_->value());
}
