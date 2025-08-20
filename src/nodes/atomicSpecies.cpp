// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "atomicSpecies.h"
#include "dissolve.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph, Elements::Element Z) : Node(parentGraph)
{
    // Create atom and AtomType
    auto &at = species_.addAtomType(Z);
    at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.3 sigma=2.0");
    species_.addAtom(Z, {}, 0.0, at);

    addPointerOutput<const Species>("Species", "Atomic species", species_);
}

std::string_view AtomicSpeciesNode::type() const { return "AtomicSpecies"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

NodeConstants::ProcessResult AtomicSpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
