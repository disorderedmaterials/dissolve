#include "atomicSpecies.h"
#include "math/vector3.h"

AtomicSpeciesNode::AtomicSpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<std::string_view>("Element", "Atomic species element symbol", Z_);
    addOption<std::string_view>("Name", "Atomic species name", name_);
    addOutput<Species *>("Species", "Atomic species", rawSpecies_);
}

std::string_view AtomicSpeciesNode::type() const { return "Atomic Species"; }

std::string_view AtomicSpeciesNode::summary() const { return "Produce an atomic species"; }

NodeConstants::ProcessResult AtomicSpeciesNode::process()
{
    rawSpecies_->addAtom(Elements::element(Z_), Vector3());
    rawSpecies_->setName(name_);

    return NodeConstants::ProcessResult::Success;
}
