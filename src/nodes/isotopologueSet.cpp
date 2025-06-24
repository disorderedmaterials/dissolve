// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/isotopologueSet.h"

IsotopologueSetNode::IsotopologueSetNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<const Species *>("Species", "Species containing the source isotopologues", species_);
    addOutput<IsotopologueSet>("IsotopologueSet", "Set/add an isotopologue and its population for a particular species",
                               isotopologueSet_);
}

std::string_view IsotopologueSetNode::type() const { return "IsotopologueSet"; }

std::string_view IsotopologueSetNode::summary() const { return "IsotopologueSet - Isotopologues for one or more Species."; }

NodeConstants::ProcessResult IsotopologueSetNode::process()
{
    // Count species isotopologues, return unchanged if none exist
    auto nIsotopologues = species_->nIsotopologues();
    if (nIsotopologues == 0)
        return NodeConstants::ProcessResult::Unchanged;

    // Iterate through species isotopologueWeights, adding isotopologue and relative weight to the set
    for (const auto &iso : species_->isotopologues())
    {
        auto index = species_->indexOfIsotopologue(iso.get());
        auto isotopologueWeight = species_->isotopologueWeight(index);
        if (isotopologueWeight)
            isotopologueSet_.add(isotopologueWeight->isotopologue(), isotopologueWeight->weight());
    }

    return NodeConstants::ProcessResult::Success;
}
