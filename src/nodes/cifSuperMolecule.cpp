// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifSuperMolecule.h"

CIFSuperMoleculeNode::CIFSuperMoleculeNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<const Species *>("NonPeriodicSpecies", "Non-periodic species", nonPeriodicSpecies_);
}

std::string_view CIFSuperMoleculeNode::type() const { return "CIFSuperMolecule"; }

std::string_view CIFSuperMoleculeNode::summary() const
{
    return "Create a single non-periodic species (useful for generating 'chunks' of crystal material)";
}

// Run main processing
NodeConstants::ProcessResult CIFSuperMoleculeNode::process()
{
    // Generate from CIF context
    context_->generate();

    // Get non-periodic species
    supercellSpecies_->copyBasic(&(context_->supercellSpecies()));
    supercellSpecies_->removePeriodicBonds();
    supercellSpecies_->removeBox();
    supercellSpecies_->updateIntramolecularTerms();
    nonPeriodicSpecies_ = supercellSpecies_.get();

    return NodeConstants::ProcessResult::Success;
}
