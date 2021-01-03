// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodereflist.h"
#include "procedure/nodes/node.h"

NodeRefListKeywordBase::NodeRefListKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
{
    parentNode_ = parentNode;
    nodeType_ = nodeType;
    onlyInScope_ = onlyInScope;
}

NodeRefListKeywordBase::~NodeRefListKeywordBase() {}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode *NodeRefListKeywordBase::parentNode() const { return parentNode_; }

/*
 * Target Node Type
 */

// Return target node type to allow
ProcedureNode::NodeType NodeRefListKeywordBase::nodeType() const { return nodeType_; }

// Return whether to accept nodes within scope only
bool NodeRefListKeywordBase::onlyInScope() const { return onlyInScope_; }
