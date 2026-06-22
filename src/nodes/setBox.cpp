// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setBox.h"

SetBoxNode::SetBoxNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<BoxContainingVariant>("Input", "Object containing box to modify", inputVariant_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<BoxContainingVariant>("Output", "Object", outputVariant_);

    // Options
    addOption<Vector3>("Lengths", "Side lengths (A, B, C) of the box (Angstroms)", lengths_);
    addOption<Vector3>("Angles", "Cell angles (alpha, beta, gamma) of the box (degrees)", angles_);
    addOption<bool>("NonPeriodic", "Whether the box should be defined as non-periodic", nonPeriodic_);
}

/*
 * Definition
 */

std::string_view SetBoxNode::type() const { return "SetBox"; }

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
                arg->createBoxAndCells(lengths_, angles_, nonPeriodic_);
            else if constexpr (std::is_same_v<T, Structure>)
                arg.createBox(lengths_, angles_, nonPeriodic_);
            else
                static_assert(false, "Visitor doesn't cater for all possible types.");

            return NodeConstants::ProcessResult::Success;
        },
        outputVariant_.data);
}
