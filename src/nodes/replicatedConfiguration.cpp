// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/replicatedConfiguration.h"
#include "math/vector3.h"

ReplicatedConfigurationNode::ReplicatedConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Source configuration to replicate", configuration_);

    // Outputs
    addPointerOutput("Configuration", "Replicated configuration", replicated_);

    // Options
    addOption("Repeat", "Integer multiples to replicate the box contents along each axis", repeat_);
}

/*
 * Definition
 */

std::string_view ReplicatedConfigurationNode::type() const { return "Replicate"; }

std::string_view ReplicatedConfigurationNode::summary() const
{
    return "Create a new configuration from repeated replicas of a source configuration";
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ReplicatedConfigurationNode::process()
{
    replicated_.empty();

    const auto &box = configuration_->box();

    // Set up configuration
    auto newLengths = box.axisLengths();
    newLengths.multiply(repeat_.x, repeat_.y, repeat_.z);
    replicated_.setBox(Box(newLengths, box.axisAngles()));

    // Loop over cell images
    for (auto ix = 0; ix < repeat_.x; ++ix)
        for (auto iy = 0; iy < repeat_.y; ++iy)
            for (auto iz = 0; iz < repeat_.z; ++iz)
            {
                // Determine translation vector
                auto delta = box.axes() * Vector3(ix, iy, iz);

                // Copy all molecules in the original configuration
                for (auto &mol : configuration_->molecules())
                    replicated_.copyMolecule(*mol)->translate(delta);
            }

    replicated_.updateObjectRelationships();

    message("Created {}x{}x{} replica - {} atoms total.\n", repeat_.x, repeat_.y, repeat_.z, replicated_.nAtoms());

    return NodeConstants::ProcessResult::Success;
}