// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/add.h"

AddNode::AddNode(Graph *parentGraph) : Node(parentGraph)
{
    addSerialisableInput<Number>("A", "First operand to the addition", a_);
    addSerialisableInput<Number>("B", "Second operand to the addition", b_);
    addOutput<Number>("Result", "The sum of the operands", result_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view AddNode::type() const { return "Add"; }

// Return short summary of the node's purpose
std::string_view AddNode::summary() const { return "Performs addition of operands A and B"; }

// Perform processing
NodeConstants::ProcessResult AddNode::process()
{
    result_ = a_ + b_;

    return NodeConstants::ProcessResult::Success;
}
