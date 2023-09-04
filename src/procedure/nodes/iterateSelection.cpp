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

    nSelectedParameter_ = parameters_.emplace_back(std::make_shared<ExpressionVariable>("nSelected"));
    siteIndexParameter_ = parameters_.emplace_back(std::make_shared<ExpressionVariable>("siteIndex"));
    stackIndexParameter_ = parameters_.emplace_back(std::make_shared<ExpressionVariable>("stackIndex"));
    indexParameter_ = parameters_.emplace_back(std::make_shared<ExpressionVariable>("index"));
}

/*
 * Identity
 */

// Set node name
void IterateSelectionProcedureNode::setName(std::string_view name)
{
    name_ = DissolveSys::niceName(name);

    // Update parameter names to match
    nSelectedParameter_->setName(fmt::format("{}.nSelected", name_));
    siteIndexParameter_->setName(fmt::format("{}.siteIndex", name_));
    stackIndexParameter_->setName(fmt::format("{}.stackIndex", name_));
    indexParameter_->setName(fmt::format("{}.index", name_));
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
    // If one exists, prepare the ForEach branch nodes
    return forEachBranch_.prepare(procedureContext);
}

// Execute node
bool IterateSelectionProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
    const auto &sites = selection_->sites();
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
    auto selectionSize = selection_->sites().size();
    // If one exists, finalise the ForEach branch nodes
    if (!forEachBranch_.finalise(procedureContext))
        return false;

    // Print out summary information
    Messenger::print("[IterateSelection] - Site '{}': Number of selections made = {} (last contained {} sites).\n", name(),
                     nSelections_, selectionSize);
    Messenger::print("[IterateSelection] - Site '{}': Average number of sites selected per selection = {:.2f}.\n", name(),
                     nSelections_ == 0 ? 0 : double(nCumulativeSites_) / nSelections_);
    Messenger::print("[IterateSelection] - Site '{}': Cumulative number of sites selected = {}.\n", name(), nCumulativeSites_);

    return true;
}