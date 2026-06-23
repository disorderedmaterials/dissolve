// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/supercellConfiguration.h"
#include "math/vector3.h"

SupercellConfigurationNode::SupercellConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Target configuration", targetConfiguration_);

    // Outputs
    addPointerOutput<Configuration>("SupercellConfiguration", "Supercell configuration", supercellConfiguration_);

    // Options
    addOption<Vector3i>("SupercellRepeat", "Integer coefficients by which unit cell will be repeated along its dimensions",
                        supercellRepeat_);
}

/*
 * Definition
 */

std::string_view SupercellConfigurationNode::type() const { return "SupercellConfiguration"; }

std::string_view SupercellConfigurationNode::summary() const
{
    return "Create a repeated instance (supercell) of a configuration";
}

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult SupercellConfigurationNode::process()
{
    supercellConfiguration_.empty();

    const auto *box = targetConfiguration_->box();

    // Set up configuration
    auto supercellLengths = box->axisLengths();
    supercellLengths.multiply(supercellRepeat_.x, supercellRepeat_.y, supercellRepeat_.z);
    supercellConfiguration_.createBoxAndCells(supercellLengths, box->axisAngles(), false);

    // Create images of all molecular unit cell species
    for (auto &mol : targetConfiguration_->molecules())
    {
        const auto *sp = mol->species();

        // Loop over cell images
        for (auto ix = 0; ix < supercellRepeat_.x; ++ix)
        {
            for (auto iy = 0; iy < supercellRepeat_.y; ++iy)
            {
                // Create and translate molecule
                for (auto iz = 0; iz < supercellRepeat_.z; ++iz)
                    supercellConfiguration_.addMolecule(sp)->translate(delta);
            }
        }
    }

    supercellConfiguration_.updateObjectRelationships();

    message("Created ({}, {}, {}) supercell - {} atoms total.\n", supercellRepeat_.x, supercellRepeat_.y, supercellRepeat_.z,
            supercellConfiguration_.nAtoms());

    return NodeConstants::ProcessResult::Success;
}