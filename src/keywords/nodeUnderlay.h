// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"

// Underlay class for ProcedureNode-based Keywords
class NodeKeywordUnderlay
{
    public:
    NodeKeywordUnderlay(ProcedureNode *parentNode, const ProcedureNode::NodeTypeVector &allowedNodeTypes, bool onlyInScope);
    virtual ~NodeKeywordUnderlay() = default;

    /*
     * Data
     */
    protected:
    // Parent ProcedureNode
    NodeRef parentNode_;
    // Optional target node types to allow
    ProcedureNode::NodeTypeVector allowedNodeTypes_;
    // Whether to accept nodes within scope only
    bool onlyInScope_;

    public:
    // Return parent ProcedureNode
    NodeRef parentNode() const;
    // Return optional target nodes type to allow
    const ProcedureNode::NodeTypeVector &allowedNodeTypes() const;
    // Return whether to accept nodes within scope only
    bool onlyInScope() const;
    // Return vector of possible nodes allowed based on class and type definitions
    std::vector<ConstNodeRef> allowedNodes() const;
    // Find the named node, obeying scope
    ConstNodeRef findNode(std::string_view name) const;
    // Return whether the supplied node has valid type
    bool validNode(const ProcedureNode *node, std::string_view keywordName) const;
    // Validate current data, returning false if invalid data had to be pruned
    virtual bool validate() = 0;
};
