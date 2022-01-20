// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/nodebase.h"

NodeKeywordBase::NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
    : parentNode_(parentNode), nodeType_(nodeType), onlyInScope_(onlyInScope)
{
}

NodeKeywordBase::NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
    : parentNode_(parentNode), nodeClass_(nodeClass), onlyInScope_(onlyInScope)
{
}

/*
 * Data
 */

// Parent ProcedureNode
ProcedureNode *NodeKeywordBase::parentNode() const { return parentNode_; }

// Return optional target node type to allow
std::optional<ProcedureNode::NodeType> NodeKeywordBase::nodeType() const { return nodeType_; }

// Return optional target node class to allow
std::optional<ProcedureNode::NodeClass> NodeKeywordBase::nodeClass() const { return nodeClass_; }

// Return whether to accept nodes within scope only
bool NodeKeywordBase::onlyInScope() const { return onlyInScope_; }

// Return whether the node has valid class or type
bool NodeKeywordBase::validNode(const ProcedureNode *node, std::optional<ProcedureNode::NodeType> nodeType,
                                std::optional<ProcedureNode::NodeClass> nodeClass, std::string_view keywordName) const
{
    // Check class (if specified) then type (if specified)
    if (nodeClass && node->nodeClass() != nodeClass.value())
        return Messenger::error("Node '{}' is of class {}, but the {} keyword requires a node of class {}.\n", node->name(),
                                ProcedureNode::nodeClasses().keyword(node->nodeClass()), keywordName,
                                ProcedureNode::nodeClasses().keyword(nodeClass.value()));

    if (nodeType && node->type() != nodeType.value())
        return Messenger::error("Node '{}' is of type {}, but the {} keyword requires a node of type {}.\n", node->name(),
                                ProcedureNode::nodeTypes().keyword(node->type()), keywordName,
                                ProcedureNode::nodeTypes().keyword(nodeType.value()));

    return true;
}
