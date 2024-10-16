// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/pickRegion.h"
#include "generator/regionBase.h"
#include "keywords/node.h"

PickRegionGeneratorNode::PickRegionGeneratorNode(std::shared_ptr<const RegionGeneratorNodeBase> region)
    : PickGeneratorNodeBase(GeneratorNode::NodeType::PickRegion), region_(region)
{
    keywords_.setOrganisation("Options", "Pick Targets");
    keywords_.add<NodeKeyword<RegionGeneratorNodeBase>>(
        "Region", "Region containing molecules that should be picked", region_, this,
        NodeTypeVector{NodeType::CylindricalRegion, NodeType::GeneralRegion, NodeType::CustomRegion});
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool PickRegionGeneratorNode::prepare(const GeneratorContext &generatorContext)
{
    if (!region_)
        return Messenger::error("A region must be supplied to PickRegion.\n");

    return true;
}

// Execute node
bool PickRegionGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    Messenger::print("[PickRegion] Molecules will be selected from {}.\n", moleculePoolName());
    Messenger::print("[PickRegion] Target region is '{}'.\n", region_->name());

    // Clear our molecules vector
    pickedMolecules_.clear();

    // Get the updated region
    auto &region = region_->region();
    if (!region.isValid())
    {
        Messenger::warn("Region will not capture any molecules...\n");
        return true;
    }

    // Loop over all molecules in supplied Configuration
    for (const auto &mol : moleculePool(generatorContext.configuration()))
        if (region.validCoordinate(mol->centreOfGeometry(generatorContext.configuration()->box())))
            pickedMolecules_.push_back(mol);

    Messenger::print("[PickRegion] Total molecules picked = {}.\n", pickedMolecules_.size());

    return true;
}
