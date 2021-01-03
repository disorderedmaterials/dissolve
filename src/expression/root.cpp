// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "expression/root.h"

ExpressionRootNode::ExpressionRootNode() : ExpressionNode() {}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionRootNode::duplicate()
{
    auto node = std::make_shared<ExpressionRootNode>();
    for (auto child : children_)
        node->addChild(child->duplicate());

    return node;
}

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
