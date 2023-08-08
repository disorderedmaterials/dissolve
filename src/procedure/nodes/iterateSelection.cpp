// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/iterateSelection.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/siteReference.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "keywords/node.h"
#include "keywords/nodeBranch.h"
#include "keywords/nodeVector.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "procedure/nodes/sequence.h"
#include <algorithm>

IterateSelectionProcedureNode::IterateSelectionProcedureNode(std::vector<const SpeciesSite *> sites,
                                                             ProcedureNode::NodeContext forEachContext)
    : ProcedureNode(ProcedureNode::NodeType::Select, {ProcedureNode::AnalysisContext, ProcedureNode::GenerationContext}),
      speciesSites_(std::move(sites)))
{
    // Keywords
}

/*
 * Selection Targets
 */

// Return vector of sites to select
std::vector<const SpeciesSite *> &IterateSelectionProcedureNode::speciesSites() { return speciesSites_; }

/*
 * Selected Sites
 */

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> IterateSelectionProcedureNode::branch() { return forEachBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool IterateSelectionProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Check for at least one site being defined
    if (speciesSites_.empty())
        return Messenger::error("No sites are defined in the Select node '{}'.\n", name());

    // If one exists, prepare the ForEach branch nodes
    if (!forEachBranch_.prepare(procedureContext))
        return false;

    return true;
}

// Execute node
bool IterateSelectionProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // Create our site vector
    sites_.clear();

    return true;
}
