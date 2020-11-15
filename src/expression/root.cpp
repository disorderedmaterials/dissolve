// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/root.h"

ExpressionRootNode::ExpressionRootNode() : ExpressionNode() {}

/*
 * Evaluation
 */

// Evaluate node
std::optional<ExpressionValue> ExpressionRootNode::evaluate() const
{
    // Must be only a single child node
    if (children_.size() != 1)
        return std::nullopt;

    return children_[0]->evaluate();
}
