// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"

// Underlay class for GeneratorNode-based Keywords
class NodeKeywordUnderlay
{
    public:
    NodeKeywordUnderlay(GeneratorNode *parentNode, const GeneratorNode::NodeTypeVector &allowedNodeTypes);
    virtual ~NodeKeywordUnderlay() = default;

    /*
     * Data
     */
    protected:
    // Parent GeneratorNode
    NodeRef parentNode_;
    // Optional target node types to allow
    GeneratorNode::NodeTypeVector allowedNodeTypes_;

    public:
    // Return parent GeneratorNode
    NodeRef parentNode() const;
    // Return optional target nodes type to allow
    const GeneratorNode::NodeTypeVector &allowedNodeTypes() const;
    // Return vector of possible nodes allowed based on class and type definitions
    std::vector<ConstNodeRef> allowedNodes() const;
    // Find the named node, obeying scope
    ConstNodeRef findNode(std::string_view name) const;
    // Return whether the supplied node has valid type
    bool validNode(const GeneratorNode *node, std::string_view keywordName) const;
    // Validate current data, returning false if invalid data had to be pruned
    virtual bool validate() = 0;
};
