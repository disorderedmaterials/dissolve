// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifPeriodicFramework.h"

CIFPeriodicFrameworkNode::CIFPeriodicFrameworkNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<const Species *>("FrameworkSpecies", "Species", frameworkSpecies_);
    addOutput<Configuration *>("SupercellConfiguration", "Supercell configuration pointer", supercellConfiguration_);

    // Options
    addOption<Vector3i>("SupercellRepeat", "Supercell repeat", supercellRepeat_);
}

std::string_view CIFPeriodicFrameworkNode::type() const { return "CIFPeriodicFramework"; }

std::string_view CIFPeriodicFrameworkNode::summary() const
{
    return "Create a single species with a periodic box and all atoms in the unit cell (suitable for framework-style models)";
}

// Run main processing
NodeConstants::ProcessResult CIFPeriodicFrameworkNode::process()
{
    // Generate from CIF context
    context_->setSupercellRepeat(supercellRepeat_);
    context_->generate();

    // Get supercell configuration
    supercellConfiguration_ = context_->generatedConfiguration();
    supercellConfiguration_->setName(context_->chemicalFormula());

    // Get framework species
    supercellSpecies_->copyBasic(&(context_->supercellSpecies()));
    supercellSpecies_->updateIntramolecularTerms();
    frameworkSpecies_ = supercellSpecies_.get();

    return NodeConstants::ProcessResult::Success;
}

// Get cleaned unit cell species
const Species &CIFPeriodicFrameworkNode::cleanedUnitCellSpecies() const { return context_->cleanedUnitCellSpecies(); }