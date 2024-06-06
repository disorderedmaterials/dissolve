// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/transmute.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "keywords/node.h"
#include "keywords/species.h"
#include "keywords/speciesVector.h"
#include "procedure/nodes/pick.h"

TransmuteProcedureNode::TransmuteProcedureNode() : ProcedureNode(NodeType::Transmute)
{
    keywords_.setOrganisation("Options", "Source");
    keywords_.add<SpeciesVectorKeyword>("Species", "Species types to transmute into the target species", speciesToTransmute_);
    keywords_.add<NodeKeyword<PickProcedureNodeBase>>(
        "Selection", "Picked selection of molecules to transmute", selection_, this,
        NodeTypeVector{NodeType::Pick, NodeType::PickProximity, NodeType::PickRegion}, true);

    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesKeyword>("Target", "Target species to transmute selected molecules in to", targetSpecies_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool TransmuteProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node
bool TransmuteProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // Check target species
    if (!targetSpecies_)
        return Messenger::error("Species to transmute into must be provided.\n");

    // Assemble a vector of molecules to transmute
    std::vector<std::shared_ptr<Molecule>> targets;

    // Transmute molecules by Species type
    if (!speciesToTransmute_.empty())
        for (const auto &mol : procedureContext.configuration()->molecules())
            if (std::find(speciesToTransmute_.begin(), speciesToTransmute_.end(), mol->species()) != speciesToTransmute_.end())
                targets.push_back(mol);

    // Transmute molecules by selection
    if (selection_)
    {
        auto pickNode = std::dynamic_pointer_cast<const PickProcedureNodeBase>(selection_);
        assert(pickNode);
        std::copy(pickNode->pickedMolecules().begin(), pickNode->pickedMolecules().end(), std::back_inserter(targets));
    }

    // Perform the magic
    const auto *box = procedureContext.configuration()->box();
    for (const auto &mol : targets)
    {
        auto newMol = procedureContext.configuration()->addMolecule(targetSpecies_);
        newMol->setCentreOfGeometry(box, mol->centreOfGeometry(box));
    }

    // Remove the old molecules
    procedureContext.configuration()->removeMolecules(targets);

    Messenger::print("[Transmute] Transmuted {} molecules into '{}'.\n", targets.size(), targetSpecies_->name());

    return true;
}
