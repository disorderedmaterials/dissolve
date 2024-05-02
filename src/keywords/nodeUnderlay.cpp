// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/nodeUnderlay.h"
#include "procedure/procedure.h"

NodeKeywordUnderlay::NodeKeywordUnderlay(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
    : parentNode_(parentNode), nodeType_(nodeType), onlyInScope_(onlyInScope)
{
}

NodeKeywordUnderlay::NodeKeywordUnderlay(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
    : parentNode_(parentNode), nodeClass_(nodeClass), onlyInScope_(onlyInScope)
{
}

/*
 * Data
 */

// Parent ProcedureNode
NodeRef NodeKeywordUnderlay::parentNode() const { return parentNode_; }

// Return optional target node type to allow
std::optional<ProcedureNode::NodeType> NodeKeywordUnderlay::nodeType() const { return nodeType_; }

// Return optional target node class to allow
std::optional<ProcedureNode::NodeClass> NodeKeywordUnderlay::nodeClass() const { return nodeClass_; }

// Return whether to accept nodes within scope only
bool NodeKeywordUnderlay::onlyInScope() const { return onlyInScope_; }

// Return vector of possible nodes allowed in the vector
std::vector<ConstNodeRef> NodeKeywordUnderlay::allowedNodes() const
{
    assert(parentNode_);
    return parentNode_->getNodes(onlyInScope_, nodeType_, nodeClass_);
}

// Find the named node, obeying scope
ConstNodeRef NodeKeywordUnderlay::findNode(std::string_view name) const
{
    assert(parentNode_);
    return parentNode_->getNode(name, onlyInScope_);
}

// Return whether the node has valid class or type
bool NodeKeywordUnderlay::validNode(const ProcedureNode *node, std::string_view keywordName) const
{
    // A null node is valid
    if (!node)
        return true;

    // Check class (if specified) then type (if specified)
    if (nodeClass_ && node->nodeClass() != nodeClass_.value())
        return Messenger::error("Node '{}' is of class {}, but the {} keyword requires a node of class {}.\n", node->name(),
                                ProcedureNode::nodeClasses().keyword(node->nodeClass()), keywordName,
                                ProcedureNode::nodeClasses().keyword(nodeClass_.value()));

    if (nodeType_ && node->type() != nodeType_.value())
        return Messenger::error("Node '{}' is of type {}, but the {} keyword requires a node of type {}.\n", node->name(),
                                ProcedureNode::nodeTypes().keyword(node->type()), keywordName,
                                ProcedureNode::nodeTypes().keyword(nodeType_.value()));

    if (!parentNode_->getNode(node->name(), onlyInScope_, {}, nodeType_, nodeClass_))
        return Messenger::error("Node '{}' does not exist (in scope), so the {} keyword cannot reference it.\n", node->name(),
                                keywordName);

    return true;
}
