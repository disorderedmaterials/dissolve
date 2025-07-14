#include "atomicSpecies.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph, Elements::Element Z) : Node(parentGraph)
{
    // Create atom and AtomType
    auto at = atomTypes_.emplace_back(std::make_shared<AtomType>(Z));
    at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.3 sigma=2.0");
    species_.addAtom(Z, {}, 0.0, at);

    addPointerOutput<const Species>("Species", "Atomic species", species_);
}

std::string_view AtomicSpeciesNode::type() const { return "AtomicSpecies"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

NodeConstants::ProcessResult AtomicSpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
