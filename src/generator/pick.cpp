// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/pick.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "keywords/speciesVector.h"

PickGeneratorNode::PickGeneratorNode(std::vector<const Species *> species)
    : PickGeneratorNodeBase(GeneratorNode::NodeType::Pick), speciesToPick_(std::move(species))
{
    keywords_.setOrganisation("Options", "Pick Targets");
    keywords_.add<SpeciesVectorKeyword>("Species", "Add target species to pick", speciesToPick_);
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool PickGeneratorNode::prepare(const GeneratorContext &generatorContext)
{
    // Check for at least one site being defined
    if (speciesToPick_.empty())
        return Messenger::error("No sites are defined in the Pick node '{}'.\n", name());

    return true;
}

// Execute node
bool PickGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    Messenger::print("[Pick] Molecules will be selected from {}.\n", moleculePoolName());

    // Create our molecules vector
    pickedMolecules_.clear();

    // Loop over all molecules in supplied Configuration
    for (const auto &mol : moleculePool(generatorContext.configuration()))
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
