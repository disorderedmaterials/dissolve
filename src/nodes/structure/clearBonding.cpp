// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/clearBonding.h"

ClearBondingNode::ClearBondingNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Structure", "Input structure", inputStructure_);

    // Outputs
    addOutput("Structure", "Modified structure", outputStructure_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ClearBondingNode::type() const { return "ClearBonding"; }

// Return short summary of the node's purpose
std::string_view ClearBondingNode::summary() const { return "Clear bonding information from a structure"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ClearBondingNode::process()
{
    outputStructure_ = inputStructure_;
    outputStructure_.clearBonds();

    return NodeConstants::ProcessResult::Success;
}