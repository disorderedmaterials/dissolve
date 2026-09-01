// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/instantiate.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "math/mathFunc.h"
#include "nodes/dissolve.h"
#include <random>

InstantiateNode::InstantiateNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Species", "Source species or molecule set to add", species_);
    addInput("Configuration", "Target configuration to insert into", configuration_);

    // Options
    addOption("ApplyBox", "Whether to apply the periodic box of the species to the configuration (if it exists)", applyBox_);

    // Outputs
    addOutput("Configuration", "Modified configuration", configuration_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view InstantiateNode::type() const { return "Instantiate"; }

// Return short summary of the node's purpose
std::string_view InstantiateNode::summary() const { return "Instantiate molecules according to defined instances"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult InstantiateNode::process()
{
    // Apply box?
    if (applyBox_ && species_->box().type() != Box::BoxType::None)
    {
        configuration_->setBox(species_->box());
    }

    // Get species instances
    const auto &instances = species_->instances();

    // Reserve space in the configuration for our new atoms
    const auto &box = configuration_->box();
    configuration_->atoms().reserve(configuration_->atoms().size() + species_->nAtoms() * instances.size());

    // Perform the insertion
    for (auto &instance : instances)
    {
        // Add the Molecule
        auto mol = configuration_->addMolecule(species_);
        mol->setAtomCoordinates(instance);
    }

    message("New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n", configuration_->atomicDensity().value_or(0.0),
            configuration_->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so Need to update our object relationships
    configuration_->updateObjectRelationships();

    return NodeConstants::ProcessResult::Success;
}
