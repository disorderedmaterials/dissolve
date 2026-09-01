// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/species.h"
#include "nodes/constants.h"

SpeciesNode::SpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput("Structure", "Structure of the species", structure_)->setFlags(ParameterBase::ParameterFlags::Required);
    addInput("Recipe", "Forcefield recipe to apply to the species", recipe_);

    addPointerOutput<const Species>("Species", "Created species", species_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view SpeciesNode::type() const { return "Species"; }

// Return short summary of the node's purpose
std::string_view SpeciesNode::summary() const { return "Produce a species"; }

/*
 * Data
 */

// Return the species object
Species &SpeciesNode::species() { return species_; }
const Species &SpeciesNode::species() const { return species_; }

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

/*
 * Serialisation
 */

// Serialise any hidden content
void SpeciesNode::serialiseInternal(SerialisedValue &target) const { species_.serialise("species", target); }

// Deserialise any hidden content
void SpeciesNode::deserialiseInternal(const SerialisedValue &node) { species_.deserialise(node.at("species")); }
