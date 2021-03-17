// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodearray.h"
#include "procedure/nodes/node.h"

NodeArrayKeywordBase::NodeArrayKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope,
                                           bool uniqueNodes)
{
    parentNode_ = parentNode;
    nodeType_ = nodeType;
    onlyInScope_ = onlyInScope;
    isVariableSize_ = true;
    fixedArraySize_ = -1;
    nodesAreUnique_ = uniqueNodes;
}
NodeArrayKeywordBase::NodeArrayKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, int fixedArraySize,
                                           bool onlyInScope, bool uniqueNodes)
{
    parentNode_ = parentNode;
    nodeType_ = nodeType;
    onlyInScope_ = onlyInScope;
    isVariableSize_ = false;
    fixedArraySize_ = fixedArraySize;
    nodesAreUnique_ = uniqueNodes;
}

NodeArrayKeywordBase::~NodeArrayKeywordBase() {}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode *NodeArrayKeywordBase::parentNode() const { return parentNode_; }

/*
 * Node Type and Array Size
 */

// Return target node type to allow
ProcedureNode::NodeType NodeArrayKeywordBase::nodeType() const { return nodeType_; }

// Return whether to accept nodes within scope only
bool NodeArrayKeywordBase::onlyInScope() const { return onlyInScope_; }

// Return whether this is a variable size array
bool NodeArrayKeywordBase::isVariableSize() const { return isVariableSize_; }

// Return size of fixed array (if relevant)
int NodeArrayKeywordBase::fixedArraySize() const { return fixedArraySize_; }

// Return whether nodes in the array must all be unique
bool NodeArrayKeywordBase::nodesAreUnique() const { return nodesAreUnique_; }
