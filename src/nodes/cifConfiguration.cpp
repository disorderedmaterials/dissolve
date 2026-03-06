// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifConfiguration.h"

CIFConfigurationNode::CIFConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Outputs
    addOutput<const Species *>("UnitCellSpecies", "Cleaned unit cell", unitCellSpecies_);
    addOutput<const Species *>("SuperCellSpecies", "Supercell species", supercellSpecies_);
    addOutput<const std::vector<CIFMolecularSpecies> *>("MolecularSpecies", "Detected molecular species", molecularSpecies_);
    addOutput<Configuration *>("SuperCellConfiguration", "Supercell configuration pointer", supercellConfiguration_);

    // Options
    addOption<Vector3i>("SuperCellRepeat", "Supercell repeat", supercellRepeat_);
}

std::string_view CIFConfigurationNode::type() const { return "CIFConfiguration"; }

std::string_view CIFConfigurationNode::summary() const { return "Generate a supercell configuration from a CIF context"; }

// Run main processing
NodeConstants::ProcessResult CIFConfigurationNode::process()
{
    if (!context_)
    {
        error("CIF context is null");
        return NodeConstants::ProcessResult::Failed;
    }

    // Generate from CIF context
    context_->setSupercellRepeat(supercellRepeat_);
    context_->generate();

    // Get supercell configuration
    supercellConfiguration_ = context_->generatedConfiguration();

    // Get cleaned unit cell species
    unitCellSpecies_ = &(context_->cleanedUnitCellSpecies());

    // Get supercell species
    supercellSpecies_ = &(context_->supercellSpecies());

    // Get detected molecular species
    molecularSpecies_ = &(context_->molecularSpecies());

    return NodeConstants::ProcessResult::Success;
}
