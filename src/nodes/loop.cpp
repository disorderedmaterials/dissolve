// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph)
{
    proxyInputs().addOption("NLoops", "Number of loops (iterations) to perform", nLoops_);
    loopBacks_ = dynamic_cast<OutputsNode *>(addNode(std::make_unique<OutputsNode>(this), "LoopBacks"));
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
    auto &sources = proxyInputs().outputs();

    for (const auto& [name, param] : sources)
        loopBacks_->inputs().insert_or_assign(name, param);
}

// Release loopback by name
void LoopGraph::releaseLoopBack(const std::string &name)
{
    auto inputs = loopBacks_->inputs();
    auto it = inputs.find(name);
    if (it != inputs.end())
        inputs.erase(it);
}

// Unlink edge, releasing the loop back if one accompanies it
void LoopGraph::unlinkEdge(Edge* edge)
{
    Node::unlinkEdge(edge);
    std::string release(edge->targetInput().name());
    releaseLoopBack(release);
}

// Reset
void LoopGraph::resetLoopCounter() { loopCounter_ = 0; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult LoopGraph::process()
{
    if (loopCounter_ > 0)
    {
        auto &sources = loopBacks_->inputs();
        auto &destinations = proxyInputs().outputs();

        if (sources.size() == 0 || destinations.size() == 0)
            return NodeConstants::ProcessResult::Unchanged;

        for (const auto& [name, param] : sources)
        {
            auto it = destinations.find(name);
            if (it != destinations.end())
            {
                auto targetParam = it->second;
                auto overrideParam = param.get();
                auto assigned = targetParam->assign(overrideParam);

                if (!assigned)
                    return NodeConstants::ProcessResult::Failed;
            }
        }
    }
    else
        return NodeConstants::ProcessResult::Unchanged;

    return NodeConstants::ProcessResult::Success;
}
