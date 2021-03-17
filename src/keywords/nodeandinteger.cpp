// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodeandinteger.h"
#include "procedure/nodes/node.h"

NodeAndIntegerKeywordBase::NodeAndIntegerKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType,
                                                     bool onlyInScope)
{
    parentNode_ = parentNode;
    nodeType_ = nodeType;
    onlyInScope_ = onlyInScope;
}

NodeAndIntegerKeywordBase::~NodeAndIntegerKeywordBase() {}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode *NodeAndIntegerKeywordBase::parentNode() const { return parentNode_; }

/*
 * Target Node Type
 */

// Return target node type to allow
ProcedureNode::NodeType NodeAndIntegerKeywordBase::nodeType() const { return nodeType_; }

// Return whether to accept nodes within scope only
bool NodeAndIntegerKeywordBase::onlyInScope() const { return onlyInScope_; }
