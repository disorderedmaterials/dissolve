// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodevector.h"
#include "procedure/nodes/node.h"
#include "procedure/procedure.h"

NodeVectorKeywordBase::NodeVectorKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
    : KeywordBase(KeywordBase::NodeVectorData), parentNode_(parentNode), nodeType_(nodeType), onlyInScope_(onlyInScope)
{
}

NodeVectorKeywordBase::NodeVectorKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
    : KeywordBase(KeywordBase::NodeVectorData), parentNode_(parentNode), nodeClass_(nodeClass), onlyInScope_(onlyInScope)
{
}

/*
 * Data
 */

// Parent ProcedureNode
ProcedureNode *NodeVectorKeywordBase::parentNode() const { return parentNode_; }

// Return optional target node type to allow
std::optional<ProcedureNode::NodeType> NodeVectorKeywordBase::nodeType() const { return nodeType_; }

// Return optional target node class to allow
std::optional<ProcedureNode::NodeClass> NodeVectorKeywordBase::nodeClass() const { return nodeClass_; }

// Return whether to accept nodes within scope only
bool NodeVectorKeywordBase::onlyInScope() const { return onlyInScope_; }

// Return vector of possible nodes allowed in the vector
std::vector<const ProcedureNode *> NodeVectorKeywordBase::allowedNodes() const
{
    // Get list of available nodes of the correct type and in the relevant scope
    std::vector<const ProcedureNode *> nodes;
    if (onlyInScope_)
        nodes = parentNode_->nodesInScope(nodeType_, nodeClass_);
    else if (parentNode_->procedure())
        nodes = parentNode_->procedure()->nodes(nodeType_, nodeClass_);

    return nodes;
}

// Return whether the node has valid class or type
bool NodeVectorKeywordBase::validNode(const ProcedureNode *node, std::optional<ProcedureNode::NodeType> nodeType,
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
