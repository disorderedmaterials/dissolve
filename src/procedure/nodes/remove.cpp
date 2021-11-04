// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/remove.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/pick.h"

RemoveProcedureNode::RemoveProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Remove)
{
    // Set up keywords
    keywords_.add("Control", new SpeciesVectorKeyword(), "Species", "Target species to remove");
    keywords_.add("Control", new NodeKeyword(shared_from_this(), ProcedureNode::NodeClass::Pick, true), "Selection",
                  "Picked selection of molecules to remove");
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool RemoveProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool RemoveProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool RemoveProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    auto &species = keywords_.retrieve<std::vector<const Species *>>("Species");
    auto *node = keywords_.retrieve<const ProcedureNode *>("Selection");

    // Store current molecule population
    auto nStartMolecules = cfg->nMolecules();

    // Remove molecules by Species type
    if (!species.empty())
        for (auto *sp : species)
            cfg->removeMolecules(sp);

    // Remove molecules by selection
    if (node)
    {
        auto *pickNode = dynamic_cast<const PickProcedureNodeBase *>(node);
        assert(pickNode);
        cfg->removeMolecules(pickNode->pickedMolecules());
    }

    Messenger::print("[Remove] Removed {} molecules from configuration '{}'.\n", nStartMolecules - cfg->nMolecules(),
                     cfg->name());

    return true;
}
