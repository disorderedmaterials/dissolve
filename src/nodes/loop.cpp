// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"
#include "nodes/inputs.h"
#include "nodes/edge.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph)
{
    addOption<Number>("Iterations", "Number of iterations to perform", iterations_);
    addOption<bool>("LoopbackInvalidates", "Whether loopback edges cause iterations to be rerun on next call", loopbackInvalidates_);
}

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
    for (auto n = 0; n < iterations_.asInteger(); ++n)
    {
        // Pull edges connected to our InputsNode (not on first iteration)
        if (n > 0)
            for (auto &[inputName, edges] : proxyInputs_->inputEdges())
            {
                for (const auto edge : edges)
                {
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

        // Process child nodes, returning early if we encounter an error
        auto result = Graph::process();
        if (result == NodeConstants::ProcessResult::Failed || result == NodeConstants::ProcessResult::InputsNotSatisfied)
            return NodeConstants::ProcessResult::Failed;
    }

    // If allowing loopback data to invalidate our status, check any loopback edges for changed data
    if (loopbackInvalidates_)
    {
        for (auto &[inputName, edges] : proxyInputs_->inputEdges())
        {
            for (const auto edge : edges)
                if (edge->requiresPull())
                    upToDate_ = false;
        }
    }

    return NodeConstants::ProcessResult::Success;
}
