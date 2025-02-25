// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#ifndef ARRAY_NODE_H
#define ARRAY_NODE_H

#include "classes/site.h"
#include "generator/node.h"
#include <vector>

class ArrayNode : public GeneratorNode
{
    public:
    // Constructor
    ArrayNode(NodeType nodeType);

    // Override execute method to apply subnode to all sites
    bool execute(const GeneratorContext &generatorContext) override;

    // Add site to the array
    void addSite(const Site &site);

    // Set the subnode that operates on each site
    void setSubNode(std::shared_ptr<GeneratorNode> subNode);

    private:
    std::vector<Site> sites_;                // List of sites
    std::shared_ptr<GeneratorNode> subNode_; // The subnode applied to each site
};

#endif // ARRAY_NODE_H