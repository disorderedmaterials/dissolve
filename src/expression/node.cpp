// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "expression/node.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

ExpressionNode::~ExpressionNode() { clear(); }

/*
 * Nodes
 */

// Clear all nodes
void ExpressionNode::clear() { children_.clear(); }

// Add child node
void ExpressionNode::addChild(std::shared_ptr<ExpressionNode> node) { children_.emplace_back(node); }

// Return number of children
int ExpressionNode::nChildren() const { return children_.size(); }
