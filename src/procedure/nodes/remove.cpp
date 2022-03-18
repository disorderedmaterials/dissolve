// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/remove.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/node.h"
#include "keywords/speciesvector.h"
#include "procedure/nodes/pick.h"

RemoveProcedureNode::RemoveProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Remove)
{
    // Set up keywords
    keywords_.add<SpeciesVectorKeyword>("Control", "Species", "Target species to remove", speciesToRemove_);
    keywords_.add<NodeKeyword<PickProcedureNodeBase>>("Control", "Selection", "Picked selection of molecules to remove",
                                                      selection_, this, ProcedureNode::NodeClass::Pick, true);
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

// Execute node
bool RemoveProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // Store current molecule population
    auto nStartMolecules = procedureContext.configuration()->nMolecules();

    // Remove molecules by Species type
    if (!speciesToRemove_.empty())
        for (auto *sp : speciesToRemove_)
            procedureContext.configuration()->removeMolecules(sp);

    // Remove molecules by selection
    if (selection_)
        procedureContext.configuration()->removeMolecules(selection_->pickedMolecules());

    Messenger::print("[Remove] Removed {} molecules from configuration '{}'.\n",
                     nStartMolecules - procedureContext.configuration()->nMolecules(),
                     procedureContext.configuration()->name());

    return true;
}
