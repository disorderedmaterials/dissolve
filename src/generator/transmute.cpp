// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/transmute.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "generator/pick.h"
#include "keywords/node.h"
#include "keywords/species.h"
#include "keywords/speciesVector.h"

TransmuteGeneratorNode::TransmuteGeneratorNode() : GeneratorNode(NodeType::Transmute)
{
    keywords_.setOrganisation("Options", "Source");
    keywords_.add<SpeciesVectorKeyword>("Species", "Species types to transmute into the target species", speciesToTransmute_);
    keywords_.add<NodeKeyword<PickGeneratorNodeBase>>(
        "Selection", "Picked selection of molecules to transmute", selection_, this,
        NodeTypeVector{NodeType::Pick, NodeType::PickProximity, NodeType::PickRegion});

    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesKeyword>("Target", "Target species to transmute selected molecules in to", targetSpecies_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool TransmuteGeneratorNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node
bool TransmuteGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    // Check target species
    if (!targetSpecies_)
        return Messenger::error("Species to transmute into must be provided.\n");

    // Assemble a vector of molecules to transmute
    std::vector<std::shared_ptr<Molecule>> targets;

    // Transmute molecules by Species type
    if (!speciesToTransmute_.empty())
        for (const auto &mol : generatorContext.configuration()->molecules())
            if (std::find(speciesToTransmute_.begin(), speciesToTransmute_.end(), mol->species()) != speciesToTransmute_.end())
                targets.push_back(mol);

    // Transmute molecules by selection
    if (selection_)
    {
        auto pickNode = std::dynamic_pointer_cast<const PickGeneratorNodeBase>(selection_);
        assert(pickNode);
        std::copy(pickNode->pickedMolecules().begin(), pickNode->pickedMolecules().end(), std::back_inserter(targets));
    }

    // Perform the magic
    const auto *box = generatorContext.configuration()->box();
    for (const auto &mol : targets)
    {
        auto newMol = generatorContext.configuration()->addMolecule(targetSpecies_);
        newMol->setCentreOfGeometry(box, mol->centreOfGeometry(box));
    }

    // Remove the old molecules
    generatorContext.configuration()->removeMolecules(targets);

    Messenger::print("[Transmute] Transmuted {} molecules into '{}'.\n", targets.size(), targetSpecies_->name());

    return true;
}
