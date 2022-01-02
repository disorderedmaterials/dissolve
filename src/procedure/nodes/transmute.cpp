// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/transmute.h"
#include "classes/atomchangetoken.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/node.h"
#include "keywords/species.h"
#include "keywords/speciesvector.h"
#include "procedure/nodes/pick.h"

TransmuteProcedureNode::TransmuteProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Transmute)
{
    // Set up keywords
    keywords_.add<SpeciesKeyword>("Control", "Target", "Target species to transmute selected molecules in to", targetSpecies_);
    keywords_.add<SpeciesVectorKeyword>("Control", "Species", "Species types to transmute into the target species",
                                        speciesToTransmute_);
    keywords_.add<NodeKeyword<PickProcedureNodeBase>>("Control", "Selection", "Picked selection of molecules to transmute",
                                                      selection_, this, ProcedureNode::NodeClass::Pick, true);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool TransmuteProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool TransmuteProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool TransmuteProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                     GenericList &targetList)
{
    // Check target species
    if (!targetSpecies_)
        return Messenger::error("Species to transmute into must be provided.\n");

    // Assemble a vector of molecules to transmute
    std::vector<std::shared_ptr<Molecule>> targets;

    // Transmute molecules by Species type
    if (!speciesToTransmute_.empty())
        for (const auto &mol : cfg->molecules())
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
    const auto *box = cfg->box();
    AtomChangeToken lock(*cfg);
    for (const auto &mol : targets)
    {
        auto newMol = cfg->addMolecule(lock, targetSpecies_);
        newMol->setCentreOfGeometry(box, mol->centreOfGeometry(box));
    }

    // Remove the old molecules
    cfg->removeMolecules(targets);

    Messenger::print("[Transmute] Transmuted {} molecules into '{}'.\n", targets.size(), targetSpecies_->name());

    return true;
}
