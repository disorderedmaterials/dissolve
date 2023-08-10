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
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include <algorithm>

IterateSelectionProcedureNode::IterateSelectionProcedureNode(ProcedureNode::NodeContext forEachContext)
    : ProcedureNode(ProcedureNode::NodeType::IterateSelection,
                    {ProcedureNode::AnalysisContext, ProcedureNode::GenerationContext}),
      forEachBranch_(forEachContext, *this, "ForEach")
{
    // Keywords
    keywords_.add<NodeKeyword<SelectProcedureNode>>("Selection", "Target selection to iterate over", selection_, this,
                                                    ProcedureNode::NodeType::Select, true);
}

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
    /*     auto sites = returnSite();
        // Create our site vector
        sites.clear();
        // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
        if (!forEachBranch_.empty())
        {
            for (currentSiteIndex_ = 0; currentSiteIndex_ < sites.size(); ++currentSiteIndex_)
            {
                ++nCumulativeSites_;

                // If the branch fails at any point, return failure here.  Otherwise, continue the loop
                if (!forEachBranch_.execute(procedureContext))
                    return false;
            }
        } */
    return true;
}
