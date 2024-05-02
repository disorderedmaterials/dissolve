// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/root.h"

ExpressionRootNode::ExpressionRootNode(bool parenthesesEnclosed) : ExpressionNode(), parenthesesEnclosed_(parenthesesEnclosed)
{
}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionRootNode::duplicate()
{
    auto node = std::make_shared<ExpressionRootNode>();
    for (const auto &child : children_)
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

// Return string representation of node
std::string ExpressionRootNode::asString() const
{
    if (children_.size() == 0)
        return "";
    return parenthesesEnclosed_ ? fmt::format("({})", children_[0]->asString()) : children_[0]->asString();
}
