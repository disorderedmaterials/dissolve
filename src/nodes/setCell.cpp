// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setCell.h"

SetCellNode::SetCellNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Target configuration for the calculation", targetConfiguration_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<Configuration *>("Configuration", "Target configuration object", targetConfiguration_);

    // Options
    addOption<Vector3>("Lengths", "Specified dimensions (x, y, z) of the box in terms of side lengths", lengths_);
    addOption<Vector3>("Angles", "Specified dimensions (alpha, beta, gamma) of the box in terms of side angles", angles_);
    addOption<bool>("NonPeriodic", "Nonperiodic box type", nonPeriodic_);
}

std::string_view SetCellNode::type() const { return "Set cell"; }

std::string_view SetCellNode::summary() const { return "Set a fixed volume for a configuration unit cell box."; }

NodeConstants::ProcessResult SetCellNode::process()
{
    targetConfiguration_->createBoxAndCells(lengths_, angles_, nonPeriodic_);

    if (!targetConfiguration_->box())
        return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
