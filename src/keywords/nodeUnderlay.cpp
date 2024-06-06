// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/nodeUnderlay.h"
#include "procedure/procedure.h"
#include "templates/algorithms.h"

NodeKeywordUnderlay::NodeKeywordUnderlay(ProcedureNode *parentNode, const ProcedureNode::NodeTypeVector &allowedNodeTypes)
    : parentNode_(parentNode), allowedNodeTypes_(allowedNodeTypes)
{
}

/*
 * Data
 */

// Parent ProcedureNode
NodeRef NodeKeywordUnderlay::parentNode() const { return parentNode_; }

// Return optional target node types to allow
const ProcedureNode::NodeTypeVector &NodeKeywordUnderlay::allowedNodeTypes() const { return allowedNodeTypes_; }

// Return vector of possible nodes allowed in the vector
std::vector<ConstNodeRef> NodeKeywordUnderlay::allowedNodes() const
{
    assert(parentNode_);
    return parentNode_->getNodes(true, allowedNodeTypes_);
}

// Find the named node, obeying scope
ConstNodeRef NodeKeywordUnderlay::findNode(std::string_view name) const
{
    assert(parentNode_);
    return parentNode_->getNode(name, true);
}

// Return whether the node has valid type
bool NodeKeywordUnderlay::validNode(const ProcedureNode *node, std::string_view keywordName) const
{
    // A null node is valid
    if (!node)
        return true;

    if (!allowedNodeTypes_.empty() &&
        std::find(allowedNodeTypes_.begin(), allowedNodeTypes_.end(), node->type()) == allowedNodeTypes_.end())
        return Messenger::error("Node '{}' is of type {}, but the {} keyword requires: {}.\n", node->name(),
                                ProcedureNode::nodeTypes().keyword(node->type()), keywordName,
                                joinStrings(allowedNodeTypes_, ", ",
                                            [](const auto nodeType) { return ProcedureNode::nodeTypes().keyword(nodeType); }));

    if (!parentNode_->getNode(node->name(), true, {}, allowedNodeTypes_))
        return Messenger::error("Node '{}' does not exist in scope, so the {} keyword cannot reference it.\n", node->name(),
                                keywordName);

    return true;
}
