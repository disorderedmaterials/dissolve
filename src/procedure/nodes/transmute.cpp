// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/transmute.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomlock.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/pick.h"

TransmuteProcedureNode::TransmuteProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Transmute)
{
    // Set up keywords
    keywords_.add("Control", new SpeciesKeyword(), "Target", "Target species to transmute selected molecules in to");
    keywords_.add("Control", new SpeciesVectorKeyword(), "Species", "Species to transmute targets in to");
    keywords_.add("Control", new NodeKeyword(this, ProcedureNode::NodeClass::Pick, true), "Selection",
                  "Picked selection of molecules to transmute");
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
    auto *sp = keywords_.retrieve<const Species *>("Target");
    auto &species = keywords_.retrieve<std::vector<const Species *>>("Species");
    auto *node = keywords_.retrieve<const ProcedureNode *>("Selection");

    // Check target species
    if (!sp)
        return Messenger::error("Species to transmute into must be provided.\n");

    // Assemble a vector of molecules to transmute
    std::vector<std::shared_ptr<Molecule>> targets;

    // Transmute molecules by Species type
    if (!species.empty())
        for (const auto &mol : cfg->molecules())
            if (std::find(species.begin(), species.end(), mol->species()) != species.end())
                targets.push_back(mol);

    // Transmute molecules by selection
    if (node)
    {
        auto *pickNode = dynamic_cast<const PickProcedureNodeBase *>(node);
        assert(pickNode);
        std::copy(pickNode->pickedMolecules().begin(), pickNode->pickedMolecules().end(), std::back_inserter(targets));
    }

    // Perform the magic
    const auto *box = cfg->box();
    AtomChangeToken lock(*cfg);
    for (const auto &mol : targets)
    {
        auto newMol = cfg->addMolecule(lock, sp);
        newMol->setCentreOfGeometry(box, mol->centreOfGeometry(box));
    }

    // Remove the old molecules
    cfg->removeMolecules(targets);

    Messenger::print("[Transmute] Transmuted {} molecules into '{}'.\n", targets.size(), sp->name());

    return true;
}
