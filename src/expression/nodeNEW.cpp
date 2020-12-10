// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/nodeNEW.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

ExpressionNodeNEW::~ExpressionNodeNEW() { clear(); }

/*
 * Nodes
 */

// Clear all nodes
void ExpressionNodeNEW::clear() { children_.clear(); }

// Add child node
void ExpressionNodeNEW::addChild(std::shared_ptr<ExpressionNodeNEW> node) { children_.emplace_back(node); }

// Return number of children
int ExpressionNodeNEW::nChildren() const { return children_.size(); }
