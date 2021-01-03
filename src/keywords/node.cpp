// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/node.h"
#include "procedure/nodes/node.h"

NodeKeywordBase::NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
{
    parentNode_ = parentNode;
    nodeType_ = nodeType;
    onlyInScope_ = onlyInScope;
}

NodeKeywordBase::~NodeKeywordBase() {}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode *NodeKeywordBase::parentNode() const { return parentNode_; }

/*
 * Target Node Type
 */

// Return target node type to allow
ProcedureNode::NodeType NodeKeywordBase::nodeType() const { return nodeType_; }

// Return whether to accept nodes within scope only
bool NodeKeywordBase::onlyInScope() const { return onlyInScope_; }
