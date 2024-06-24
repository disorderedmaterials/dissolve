// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/iterateSelection.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "generator/select.h"
#include "generator/sequence.h"
#include "keywords/node.h"
#include "keywords/speciesSiteVector.h"
#include <algorithm>

IterateSelectionGeneratorNode::IterateSelectionGeneratorNode()
    : GeneratorNode(NodeType::IterateSelection), forEachBranch_(*this, "ForEach")
{
    // Keywords
    keywords_.add<NodeKeyword<SelectGeneratorNode>>("Selection", "Target selection to iterate over", selection_, this,
                                                    NodeTypeVector{NodeType::Select});

    nSelectedParameter_ = addParameter("nSelected");
    siteIndexParameter_ = addParameter("siteIndex");
    stackIndexParameter_ = addParameter("stackIndex");
    indexParameter_ = addParameter("index");
}

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<GeneratorNodeSequence> IterateSelectionGeneratorNode::branch() { return forEachBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool IterateSelectionGeneratorNode::prepare(const ProcedureContext &procedureContext)
{
    // If one exists, prepare the ForEach branch nodes
    return forEachBranch_.prepare(procedureContext);
}

// Execute node
bool IterateSelectionGeneratorNode::execute(const ProcedureContext &procedureContext)
{
    const auto &sites = selection_->sites();
    currentSite_ = std::nullopt;

    // Update parameters
    nSelectedParameter_->setValue(int(sites.size()));

    // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
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

    return true;
}
