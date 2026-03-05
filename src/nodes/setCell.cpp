// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setCell.h"

SetCellNode::SetCellNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Target configuration for the calculation", targetConfiguration_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<Configuration *>("Configuration", "Target configuration for the cell definition", targetConfiguration_);

    // Options
    addOption<Vector3>("Lengths", "Side lengths (A, B, C) of the cell (Angstroms)", lengths_);
    addOption<Vector3>("Angles", "Cell angles (alpha, beta, gamma) of the cell (degrees)", angles_);
    addOption<bool>("NonPeriodic", "Whether the cell should be defined as non-periodic", nonPeriodic_);
}

std::string_view SetCellNode::type() const { return "SetCell"; }

std::string_view SetCellNode::summary() const { return "Define / overwrite a target configuration's unit cell."; }

NodeConstants::ProcessResult SetCellNode::process()
{
    targetConfiguration_->createBoxAndCells(lengths_, angles_, nonPeriodic_);

    if (!targetConfiguration_->box())
        return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
