// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/clearBonding.h"

ClearBondingNode::ClearBondingNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Structure>("Structure", "Input structure", inputStructure_);

    // Outputs
    addOutput<Structure>("Structure", "Modified structure", outputStructure_);
}

std::string_view ClearBondingNode::type() const { return "ClearBonding"; }

std::string_view ClearBondingNode::summary() const { return "Clear bonding information from a structure"; }

// Run main processing
NodeConstants::ProcessResult ClearBondingNode::process()
{
    outputStructure_.clear();
    outputStructure_ = inputStructure_;
    outputStructure_.clearBonds();
    return NodeConstants::ProcessResult::Success;
}