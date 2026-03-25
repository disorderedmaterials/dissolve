// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setCIFAtomGroupActivity.h"

SetCIFAtomGroupActivityNode::SetCIFAtomGroupActivityNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Options
    addOption<std::string>("Assembly", "CIF assembly name", assemblyName_);
    addOption<std::string>("AtomGroup", "CIF atom group name", atomGroupName_);
    addOption<bool>("SetActive", "Activity status of selected CIF atom group", active_);
}

std::string_view SetCIFAtomGroupActivityNode::type() const { return "SetCIFAtomGroupActivity"; }

std::string_view SetCIFAtomGroupActivityNode::summary() const
{
    return "Set activity of CIF assembly atom groups and apply to a CIF context";
}

// Run main processing
NodeConstants::ProcessResult SetCIFAtomGroupActivityNode::process()
{

    auto &atomGroup = context_->getAssembly(assemblyName_).getGroup(atomGroupName_);
    atomGroup.setActive(active_);

    return NodeConstants::ProcessResult::Success;
}
