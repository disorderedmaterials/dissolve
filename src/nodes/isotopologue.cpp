// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/isotopologue.h"

IsotopologueNode::IsotopologueNode(Graph* parentGraph) : Node(parentGraph)
{
    addOption<std::string>("Name", "Isotopologue name", isoName_);
    addOption<double>("RelativeWeight", "Isotopologue relative weight", weight_);
    addPointerOutput<const Isotopologue*>("Isotopologue", "Isotopologue object", isotopologue_);
    addPointerOutput<const IsotopologueWeight*>("IsotopologueWeight", "Isotopologue weight object", isotopologueWeight_);
}

std::string_view IsotopologueNode::type() const { return "Isotopologue"; }

std::string_view IsotopologueNode::summary() const { return "Isotopologue."; }

NodeConstants::ProcessResult IsotopologueNode::process()
{
    isotopologue_.setName(name_);
    isotopologueWeight_.setIsotopologue(isotopologue_);
    isotopologueWeight_.setWeight(weight_);
    return NodeConstants::ProcessResult::Success;
}
