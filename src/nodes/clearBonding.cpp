// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/clearBonding.h"

ClearBondingNode::ClearBondingNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addOutput<Structure>("Structure", "Structure containing atoms and connectivity", structure_);

    // Outputs
    addOutput<Structure>("Structure", "Structure containing atoms without bonding", structure_);
}

std::string_view ClearBondingNode::type() const { return "ClearBonds"; }

std::string_view ClearBondingNode::summary() const { return "Clear bonding information from a structure"; }

// Run main processing
NodeConstants::ProcessResult ClearBondingNode::process()
{
    auto &bonds = structure_.bonds();
    bonds.clear();
    return NodeConstants::ProcessResult::Success;
}