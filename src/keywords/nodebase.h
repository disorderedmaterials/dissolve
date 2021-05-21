// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/data.h"
#include "procedure/nodes/node.h"

// Base Class for ProcedureNode-based Keywords
class NodeKeywordBase
{
    public:
    NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope);
    NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope);
    virtual ~NodeKeywordBase() = default;

    /*
     * Data
     */
    protected:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;
    // Optional target node type to allow
    std::optional<ProcedureNode::NodeType> nodeType_;
    // Optional target node class to allow
    std::optional<ProcedureNode::NodeClass> nodeClass_;
    // Whether to accept nodes within scope only
    bool onlyInScope_;

    public:
    // Return parent ProcedureNode
    ProcedureNode *parentNode() const;
    // Return optional target node type to allow
    std::optional<ProcedureNode::NodeType> nodeType() const;
    // Return optional target node class to allow
    std::optional<ProcedureNode::NodeClass> nodeClass() const;
    // Return whether to accept nodes within scope only
    bool onlyInScope() const;
    // Return whether the supplied node has valid class or type
    bool validNode(const ProcedureNode *node, std::optional<ProcedureNode::NodeType> nodeType,
                   std::optional<ProcedureNode::NodeClass> nodeClass, std::string_view keywordName) const;
};
