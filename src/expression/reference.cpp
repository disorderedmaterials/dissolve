// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/reference.h"
#include "expression/variable.h"

ExpressionReferenceNode::ExpressionReferenceNode(ExpressionVariable *variable) : ExpressionNodeNEW(), variable_(variable) {}

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
