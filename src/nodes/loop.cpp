// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph)
{
    loopBacks_ = dynamic_cast<OutputsNode *>(addNode(std::make_unique<OutputsNode>(this), "LoopBacks"));
    setLoopBacks();
}

/*
 * Definitions (Virtuals)
 */

// Return node name
std::string_view LoopGraph::name() const { return "Loop"; }

// Return type of the node
std::string_view LoopGraph::type() const { return "Loop"; }

// Return short summary of the node's purpose
std::string_view LoopGraph::summary() const { return "Loop the contained graph"; }

//
void LoopGraph::setLoopBacks()
{
    auto &sources = proxyInputs().inputs();

    for (const auto& [name, param] : sources)
        loopBacks_->inputs().emplace(name, param);
}

// Add supplied proxy output, setting ownership of the parameters appropriately
bool LoopGraph::addLoopBack(std::shared_ptr<ParameterBase> &input, std::shared_ptr<ParameterBase> &source)
{

    return true;
}