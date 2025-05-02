#include "atomicSpecies.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph, std::string_view name, Elements::Element Z, Vector3 r)
    : Node(parentGraph)
{
    species_.addAtom(Z, r);
    species_.setName(name);
    addOutput<const Species *>("Species", "Atomic species", rawSpecies_);
}

std::string_view AtomicSpeciesNode::type() const { return "Atomic Species"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

NodeConstants::ProcessResult AtomicSpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
