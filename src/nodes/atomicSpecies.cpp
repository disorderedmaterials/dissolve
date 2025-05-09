#include "atomicSpecies.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph, Elements::Element Z) : Node(parentGraph)
{
    species_.addAtom(Z, {});
    addPointerOutput<const Species *>("Species", "Atomic species", species_);
}

std::string_view AtomicSpeciesNode::type() const { return "AtomicSpecies"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

NodeConstants::ProcessResult AtomicSpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
