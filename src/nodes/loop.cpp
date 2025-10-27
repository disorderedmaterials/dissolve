// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"
#include "nodes/edge.h"
#include "nodes/inputs.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph)
{
    addOption<Number>("Iterations", "Number of iterations to perform", iterations_);
    addOption<bool>("LoopbackInvalidates", "Whether loopback edges cause iterations to be rerun on next call",
                    loopbackInvalidates_);
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
    // Could we have a map of inputs to the graph and when they were updated, which we maintain between runs? We can then decide
    // whether to use current parameter values or pull from loopback edges.
    for (auto n = 0; n < iterations_.asInteger(); ++n)
    {
        std::cout << std::format("\nLoopGraph::process() - iteration {}\n\n", n);

        // Pull edges connected to our InputsNode *if*
        if (n > 0)
        {
            for (auto &[inputName, edges] : proxyInputs_->inputEdges())
            {
                for (const auto edge : edges)
                {
                    std::cout << std::format("{}LoopGraph::process() - Pulling edge {}...\n", GraphDebug::indent(),
                                             edge->definition().asString());
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
        }

        // Process child nodes, returning early if we encounter an error
        auto result = Graph::process();
        if (result == NodeConstants::ProcessResult::Failed || result == NodeConstants::ProcessResult::InputsNotSatisfied)
            return NodeConstants::ProcessResult::Failed;
    }

    // // If allowing loopback data to invalidate our status, check any loopback edges for changed data
    // if (loopbackInvalidates_)
    // {
    //     for (auto &[inputName, edges] : proxyInputs_->inputEdges())
    //     {
    //         for (const auto edge : edges)
    //             if (edge->requiresPull())
    //                 return NodeConstants::ProcessResult::SuccessAndNotUpdated;
    //     }
    // }

    return NodeConstants::ProcessResult::Success;
}
