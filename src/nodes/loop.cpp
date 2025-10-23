// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"
#include "nodes/inputs.h"
#include "nodes/outputs.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph) {}

/*
 * Definitions (Virtuals)
 */

// Return type of the node
std::string_view LoopGraph::type() const { return "Loop"; }

// Return short summary of the node's purpose
std::string_view LoopGraph::summary() const { return "Loop the contained graph"; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult LoopGraph::process()
{
    /*
     * Processing a Graph involves running any child nodes we have, but we can only detect the nodes that need to be run in
     * one of two ways. Either 1) We cycle over Edge connections to inputs on our Outputs node and pull() those in, or 2) we
     * look for any nodes that don't have any edge connections to their Outputs and try to run() them one at a time. The
     * latter case is important if a Graph has no defined Outputs, and so no external dependence on running the child nodes.
     */

    // Pull proxy outputs first
    auto outputsResult = proxyOutputs_->run();
    if (outputsResult == NodeConstants::ProcessResult::Failed)
        return outputsResult;

    // Check each node for output edges - any that have zero output edges need to be run()
    auto terminalNodeResult = NodeConstants::ProcessResult::Unchanged;
    for (auto &&[nodeName, node] : nodes_)
        if (!node->outputEdges().empty())
        {
            switch (node->run())
            {
                case (NodeConstants::ProcessResult::Failed):
                    return NodeConstants::ProcessResult::Failed;
                case (NodeConstants::ProcessResult::Success):
                    terminalNodeResult = NodeConstants::ProcessResult::Success;
                    break;
                case (NodeConstants::ProcessResult::Unchanged):
                    break;
                case (NodeConstants::ProcessResult::InputsNotSatisfied):
                    /* This should never happen? */
                    break;
            }
        }

    return outputsResult == terminalNodeResult ? outputsResult : NodeConstants::ProcessResult::Success;
}

NodeConstants::ProcessResult LoopGraph::testLoopBack()
{
    // For each defined InputsNode input, pull any edges...
    // Pull all input edges. If any are out-of-date and get re-set this will automatically unset upToDate_
    for (auto &[inputName, edges] : proxyInputs_->inputEdges())
    {
        for (const auto edge : edges)
        {
            std::cout << std::format("Pulling edge '{}'..\n", edge->definition().asString());
            switch (edge->pull())
            {
                case (NodeConstants::ProcessResult::Failed):
                case (NodeConstants::ProcessResult::InputsNotSatisfied):
                    return NodeConstants::ProcessResult::Failed;
                case (NodeConstants::ProcessResult::Success):
                case (NodeConstants::ProcessResult::Unchanged):
                    break;
            }
        }
    }

    return NodeConstants::ProcessResult::Success;
}
