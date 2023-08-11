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
    // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
    auto sites = selection_->returnSites();
    currentSite_ = std::nullopt;
    if (!forEachBranch_.empty())
    {
        auto index = 1;
        for (const auto &siteInfo : sites)
        {
            currentSite_ = std::get<0>(siteInfo);
            siteIndexParameter_->setValue(std::get<1>(siteInfo));
            stackIndexParameter_->setValue(std::get<2>(siteInfo));
            indexParameter_->setValue(index++);

            ++nCumulativeSites_;

            // If the branch fails at any point, return failure here.  Otherwise, continue the loop
            if (!forEachBranch_.execute(procedureContext))
                return false;
        }
    }
    // Update parameters
    nSelectedParameter_->setValue(int(sites.size()));
    return true;
}

bool IterateSelectionProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    auto selectionSize = selection_->returnSites().size();
    // If one exists, finalise the ForEach branch nodes
    if (!forEachBranch_.finalise(procedureContext))
        return false;

    // Print out summary information
    Messenger::print("Select - Site '{}': Number of selections made = {} (last contained {} sites).\n", name(), nSelections_,
                     selectionSize);
    Messenger::print("Select - Site '{}': Average number of sites selected per selection = {:.2f}.\n", name(),
                     nSelections_ == 0 ? 0 : double(nCumulativeSites_) / nSelections_);
    Messenger::print("Select - Site '{}': Cumulative number of sites selected = {}.\n", name(), nCumulativeSites_);

    return true;
}