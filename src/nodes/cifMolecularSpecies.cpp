// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifMolecularSpecies.h"
#include <algorithm>
#include <iterator>

CIFMolecularSpeciesNode::CIFMolecularSpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies", "Detected molecular species", molecularSpecies_);
    addOutput<Configuration *>("SupercellConfiguration", "Supercell configuration pointer", supercellConfiguration_);

    // Options
    addOption<Vector3i>("SupercellRepeat", "Supercell repeat", supercellRepeat_);
}

std::string_view CIFMolecularSpeciesNode::type() const { return "CIFMolecularSpecies"; }

std::string_view CIFMolecularSpeciesNode::summary() const
{
    return "Output a configuration containing individual molecules based on detected species";
}

// Run main processing
NodeConstants::ProcessResult CIFMolecularSpeciesNode::process()
{
    // Generate from CIF context
    context_->setSupercellRepeat(supercellRepeat_);
    context_->generate();

    // Get supercell configuration
    supercellConfiguration_ = context_->generatedConfiguration();
    supercellConfiguration_->setName(context_->chemicalFormula());

    // Get detected molecular species
    auto &cifMols = context_->molecularSpecies();
    molecularSpecies_.clear();
    std::ranges::copy(cifMols, std::back_inserter(molecularSpecies_));

    return NodeConstants::ProcessResult::Success;
}

// Get cleaned unit cell species
const Species &CIFMolecularSpeciesNode::cleanedUnitCellSpecies() const { return context_->cleanedUnitCellSpecies(); }