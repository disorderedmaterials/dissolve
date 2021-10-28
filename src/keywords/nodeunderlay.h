// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"

// Underlay class for ProcedureNode-based Keywords
class NodeKeywordUnderlay
{
    public:
    NodeKeywordUnderlay(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope);
    NodeKeywordUnderlay(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope);
    virtual ~NodeKeywordUnderlay() = default;

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
    // Return vector of possible nodes allowed based on class and type definitions
    std::vector<const ProcedureNode *> allowedNodes() const;
    // Find the named node, obeying scope
    const ProcedureNode *findNode(std::string_view name) const;
    // Return whether the supplied node has valid class or type
    bool validNode(const ProcedureNode *node, std::string_view keywordName) const;
};
