// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/test.h"

TestNode::TestNode(Graph *parentGraph) : Node(parentGraph)
{
    addOptionalPointerOutput<Number>("A", "First operand to the addition", a_);
    addInput<Number>("B", "Second operand to the addition", b_);
    addOutput<Number>("Result", "The sum of the operands", result_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view TestNode::type() const { return "TestNode"; }

// Return short summary of the node's purpose
std::string_view TestNode::summary() const { return "A node to allow unit testing of various graph features"; }

// Perform processing
NodeConstants::ProcessResult TestNode::process()
{
    result_ = a_ + b_;

    return NodeConstants::ProcessResult::Success;
}
