// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/atomicSpecies.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("Element", "Species atomic element symbol", element_);
    addOption("InteractionPotential",
              "String containing the parameters for the short range function describing the species interaction potential",
              parameters_);

    // Outputs
    addPointerOutput<const Species>("Species", "Created species", species_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view AtomicSpeciesNode::type() const { return "AtomicSpecies"; }

// Return short summary of the node's purpose
std::string_view AtomicSpeciesNode::summary() const { return "Create atomic species"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult AtomicSpeciesNode::process()
{
    auto potential = parameters_.empty()
                         ? InteractionPotential<ShortRangeFunctions>{ShortRangeFunctions::Form::Undefined}
                         : InteractionPotential<ShortRangeFunctions>{ShortRangeFunctions::Form::LennardJones, parameters_};
    species_.setName(Elements::symbol(element_));
    species_.createAtomic(element_, potential);
    return NodeConstants::ProcessResult::Success;
}
