// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/isotopologueSet.h"

IsotopologueSetNode::IsotopologueSetNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<const Species *>("Species", "Atomic species", species_);
    addOutput<IsotopologueSet>("IsotopologueSet", "Set/add an isotopologue and its population for a particular species",
                               isotopologueSet_);
}

std::string_view IsotopologueSetNode::type() const { return "IsotopologueSet"; }

std::string_view IsotopologueSetNode::summary() const { return "IsotopologueSet - Isotopologues for one or more Species."; }

NodeConstants::ProcessResult IsotopologueSetNode::process() { return NodeConstants::ProcessResult::Success; }
