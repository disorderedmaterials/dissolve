// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/pick.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/types.h"

PickProcedureNode::PickProcedureNode(std::vector<const Species *> species)
    : PickProcedureNodeBase(ProcedureNode::NodeType::Pick)
{
    keywords_.add("Control", new SpeciesVectorKeyword(std::move(species)), "Species", "Add target species to pick");
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool PickProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve target Species from keyword
    targetSpecies_ = keywords_.retrieve<std::vector<const Species *>>("Species");

    // Check for at least one site being defined
    if (targetSpecies_.empty())
        return Messenger::error("No sites are defined in the Select node '{}'.\n", name());

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
        if (std::find(targetSpecies_.begin(), targetSpecies_.end(), mol->species()) == targetSpecies_.end())
            continue;

        // Pick it!
        pickedMolecules_.push_back(mol);
    }

    Messenger::print("[Pick] Total molecules picked = {}.\n", pickedMolecules_.size());

    return true;
}
