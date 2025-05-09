#include "atomicSpecies.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph, std::string_view name, Elements::Element Z) : Node(parentGraph)
{
    species_.addAtom(Z, {});
    species_.setName(name);
    addPointerOutput<const Species *>("Species", "Atomic species", species_);
}

std::string_view AtomicSpeciesNode::type() const { return "Atomic Species"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

NodeConstants::ProcessResult AtomicSpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
