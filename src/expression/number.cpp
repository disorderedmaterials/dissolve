// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/number.h"

ExpressionNumberNode::ExpressionNumberNode(int i) : ExpressionNodeNEW() { value_ = i; }

ExpressionNumberNode::ExpressionNumberNode(double d) : ExpressionNodeNEW() { value_ = d; }

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
