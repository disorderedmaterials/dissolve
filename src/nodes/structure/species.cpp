// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/species.h"
#include "nodes/constants.h"

SpeciesNode::SpeciesNode(Graph *parentGraph) : SpeciesBaseNode(parentGraph)
{
    // Inputs
    addInput("Structure", "Structure of the species", structure_)->setFlags(ParameterBase::ParameterFlags::Required);
    addInput("Recipe", "Forcefield recipe to apply to the species", recipe_);

    // State data
    addStateData("Species", species_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view SpeciesNode::type() const { return "Species"; }

// Return short summary of the node's purpose
std::string_view SpeciesNode::summary() const { return "Produce a species"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult SpeciesNode::process()
{
    if (structure_)
        species_.create(*structure_);

    if (recipe_)
        return recipe_->apply(species_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
