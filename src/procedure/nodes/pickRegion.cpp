// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/pickRegion.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/node.h"
#include "procedure/nodes/regionBase.h"

PickRegionProcedureNode::PickRegionProcedureNode(std::shared_ptr<const RegionProcedureNodeBase> region)
    : PickProcedureNodeBase(ProcedureNode::NodeType::PickRegion), region_(region)
{
    keywords_.setOrganisation("Options", "Pick Targets");
    keywords_.add<NodeKeyword<RegionProcedureNodeBase>>("Region", "Region containing molecules that should be picked", region_,
                                                        this, ProcedureNode::NodeClass::Region, true);
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool PickRegionProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!region_)
        return Messenger::error("A region must be supplied to PickRegion.\n");

    return true;
}

// Execute node
bool PickRegionProcedureNode::execute(const ProcedureContext &procedureContext)
{
    Messenger::print("[PickRegion] Molecules will be selected from {}.\n", moleculePoolName());
    Messenger::print("[PickRegion] Target region is '{}'.\n", region_->name());

    // Create our molecules vector
    pickedMolecules_.clear();

    // Get the updated region
    auto &region = region_->region();
    if (!region.isValid())
    {
        Messenger::warn("Region will not capture any molecules...\n");
        return true;
    }

    // Loop over all molecules in supplied Configuration
    for (const auto &mol : moleculePool(procedureContext.configuration()))
        if (region.validCoordinate(mol->centreOfGeometry(procedureContext.configuration()->box())))
            pickedMolecules_.push_back(mol);

    Messenger::print("[PickRegion] Total molecules picked = {}.\n", pickedMolecules_.size());

    return true;
}
