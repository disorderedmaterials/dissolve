// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/add.h"

AddNode::AddNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addSerialisableInput<Number>("X", "First operand to the addition", x_);
    addSerialisableInput<Number>("Y", "Second operand to the addition", y_);

    // Outputs
    addOutput("Result", "The sum of the operands", result_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view AddNode::type() const { return "Add"; }

// Return short summary of the node's purpose
std::string_view AddNode::summary() const { return "Performs addition of operands X and Y"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult AddNode::process()
{
    result_ = x_ + y_;

    return NodeConstants::ProcessResult::Success;
}
