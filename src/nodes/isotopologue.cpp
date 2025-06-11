// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/isotopologue.h"

IsotopologueNode::IsotopologueNode(Graph *parentGraph) : Node(parentGraph)
{
    addOutput<IsotopologueSet>("Isotopologue", "Set/add an isotopologue and its population for a particular species",
                               isotopologueSet_);
}

std::string_view IsotopologueNode::type() const { return "Isotopologue"; }

std::string_view IsotopologueNode::summary() const { return "Isotopologue."; }

NodeConstants::ProcessResult IsotopologueNode::process()
{
    isotopologueSet_.add(&iso_, relativeWeight_);

    return NodeConstants::ProcessResult::Success;
}
