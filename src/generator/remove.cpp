// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/remove.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "generator/pick.h"
#include "keywords/node.h"
#include "keywords/speciesVector.h"

RemoveGeneratorNode::RemoveGeneratorNode() : GeneratorNode(NodeType::Remove)
{
    keywords_.setOrganisation("Options", "Targets");
    keywords_.add<SpeciesVectorKeyword>("Species", "Target species to remove", speciesToRemove_);
    keywords_.add<NodeKeyword<PickGeneratorNodeBase>>(
        "Selection", "Picked selection of molecules to remove", selection_, this,
        NodeTypeVector{NodeType::Pick, NodeType::PickProximity, NodeType::PickRegion});
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RemoveGeneratorNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node
bool RemoveGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    // Store current molecule population
    auto nStartMolecules = generatorContext.configuration()->nMolecules();

    // Remove molecules by Species type
    if (!speciesToRemove_.empty())
        for (auto *sp : speciesToRemove_)
            generatorContext.configuration()->removeMolecules(sp);

    // Remove molecules by selection
    if (selection_)
        generatorContext.configuration()->removeMolecules(selection_->pickedMolecules());

    Messenger::print("[Remove] Removed {} molecules from configuration '{}'.\n",
                     nStartMolecules - generatorContext.configuration()->nMolecules(),
                     generatorContext.configuration()->name());

    return true;
}
