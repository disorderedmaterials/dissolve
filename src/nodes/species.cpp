// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "species.h"
#include "nodes/constants.h"
#include <toml11/toml.hpp>
#include <toml11/toml/parser.hpp>

SpeciesNode::SpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput("Structure", "Structure of the species", structure_);
    addInput("Recipe", "Forcefield recipe to apply to the species", recipe_);

    addPointerOutput<const Species>("Species", "Created species", species_);
}

std::string_view SpeciesNode::type() const { return "Species"; }

std::string_view SpeciesNode::summary() const { return "Produce a species"; }

NodeConstants::ProcessResult SpeciesNode::process()
{
    if (structure_ && recipe_)
    {
        species_.create(*structure_);
        return recipe_->apply(species_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Unchanged;
}

Species &SpeciesNode::species() { return species_; }
const Species &SpeciesNode::species() const { return species_; }

// Serialise any hidden content
void SpeciesNode::serialiseInternal(SerialisedValue &target) const { species_.serialise("species", target); }

// Deserialise any hidden content
void SpeciesNode::deserialiseInternal(const SerialisedValue &node) { species_.deserialise(node.at("species")); }
