// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "generator/node.h"
#include <memory>
#include <vector>

class ArrayNode : public GeneratorNode
{
    public:
    ArrayNode(NodeType nodeType);

    void addSite(const Site &site);
    void setSubNode(std::shared_ptr<GeneratorNode> subNode);
    bool execute(const GeneratorContext &generatorContext) override;

    private:
    std::vector<Site> sites_;
    std::shared_ptr<GeneratorNode> subNode_;
};