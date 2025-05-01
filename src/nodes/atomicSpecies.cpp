#include "atomicSpecies.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph, std::string_view name, Elements::Element Z, Vector3 r)
    : Node(parentGraph)
{
    reset(name, Z, r);
    addOutput<const Species *>("Species", "Atomic species", rawSpecies_);
}

std::string_view AtomicSpeciesNode::type() const { return "Atomic Species"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

void AtomicSpeciesNode::reset(std::string_view name, Elements::Element Z, Vector3 r)
{
    species_.addAtom(Z, r);
    species_.setName(name);
    rawSpecies_ = &species_;
}

NodeConstants::ProcessResult AtomicSpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
