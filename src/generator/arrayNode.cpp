// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "generator/arrayNode.h"

ArrayNode::ArrayNode(NodeType nodeType) : GeneratorNode(nodeType) {}

void ArrayNode::addSite(const Site &site) { sites_.push_back(site); }

void ArrayNode::setSubNode(std::shared_ptr<GeneratorNode> subNode) { subNode_ = subNode; }

bool ArrayNode::execute(const GeneratorContext &generatorContext)
{
    if (!subNode_) // Ensure a subnode exists
        return false;

    for (auto &site : sites_)
    {
        // Set the site's position in the subnode's parameters
        subNode_->addParameter("position", site.position());

        // Execute the subnode on this site
        if (!subNode_->execute(generatorContext))
            return false;
    }
    return true;
}
