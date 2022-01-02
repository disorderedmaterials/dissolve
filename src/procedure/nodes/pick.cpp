// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/pick.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/speciesvector.h"

PickProcedureNode::PickProcedureNode(std::vector<const Species *> species)
    : PickProcedureNodeBase(ProcedureNode::NodeType::Pick), speciesToPick_(std::move(species))
{
    keywords_.add<SpeciesVectorKeyword>("Control", "Species", "Add target species to pick", speciesToPick_);
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool PickProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Check for at least one site being defined
    if (speciesToPick_.empty())
        return Messenger::error("No sites are defined in the Pick node '{}'.\n", name());

    return true;
}

// Execute node, targetting the supplied Configuration
bool PickProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    Messenger::print("[Pick] Molecules will be selected from {}.\n", moleculePoolName());

    // Create our molecules vector
    pickedMolecules_.clear();

    // Loop over all molecules in supplied Configuration
    for (const auto &mol : moleculePool(cfg))
    {
        // Check Species type
        if (std::find(speciesToPick_.begin(), speciesToPick_.end(), mol->species()) == speciesToPick_.end())
            continue;

        // Pick it!
        pickedMolecules_.push_back(mol);
    }

    Messenger::print("[Pick] Total molecules picked = {}.\n", pickedMolecules_.size());

    return true;
}
