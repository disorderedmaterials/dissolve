// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setCell.h"

SetCellNode::SetCellNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CellContainingVariant>("Input", "Target configuration for the calculation", inputVariant_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<CellContainingVariant>("Output", "Target configuration for the cell definition", outputVariant_);

    // Options
    addOption<Vector3>("Lengths", "Side lengths (A, B, C) of the cell (Angstroms)", lengths_);
    addOption<Vector3>("Angles", "Cell angles (alpha, beta, gamma) of the cell (degrees)", angles_);
    addOption<bool>("NonPeriodic", "Whether the cell should be defined as non-periodic", nonPeriodic_);
}

std::string_view SetCellNode::type() const { return "SetCell"; }

std::string_view SetCellNode::summary() const { return "Define / overwrite a target configuration's unit cell."; }

NodeConstants::ProcessResult SetCellNode::process()
{
    // Copy the input to the output and work on the output
    outputVariant_ = inputVariant_;

    std::visit(
        [&](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Configuration *>)
                arg->createBoxAndCells(lengths_, angles_, nonPeriodic_);
            else if constexpr (std::is_same_v<T, Structure>)
                arg.createBox(lengths_, angles_, nonPeriodic_);
            else
                static_assert(false, "Visitor doesn't cater for all possible types.");
        },
        outputVariant_);

    return NodeConstants::ProcessResult::Success;
}
