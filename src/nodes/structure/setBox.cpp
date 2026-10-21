// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setBox.h"

SetBoxNode::SetBoxNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Input", "Object containing box to modify", inputVariant_)->setFlags({ParameterBase::Required});

    // Outputs
    addOutput("Output", "Object", outputVariant_);

    // Options
    addOption("Lengths", "Side lengths (A, B, C) of the box (Angstroms)", lengths_);
    addOption("Angles", "Cell angles (alpha, beta, gamma) of the box (degrees)", angles_);
    addOption("NonPeriodic", "Whether the box should be defined as non-periodic", nonPeriodic_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view SetBoxNode::type() const { return "SetBox"; }

// Return short summary of the node's purpose
std::string_view SetBoxNode::summary() const { return "Set the periodic box definition on an object."; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult SetBoxNode::process()
{
    // Copy the input to the output and work on the output
    outputVariant_ = inputVariant_;

    return std::visit(
        [&](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>)
                return NodeConstants::ProcessResult::Failed;
            else if constexpr (std::is_same_v<T, Configuration *>)
                arg->setBox(Box(lengths_, angles_));
            else if constexpr (std::is_same_v<T, Structure>)
                arg.box() = Box(lengths_, angles_);
            else
                static_assert(false, "Visitor doesn't cater for all possible types.");

            return NodeConstants::ProcessResult::Success;
        },
        outputVariant_.data);
}
