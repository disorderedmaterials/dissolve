// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/numberNode.h"

NumberNode::NumberNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<Number>("X", "A number", a_);
    addOutput<Number>("X", "A number", a_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view NumberNode::type() const { return "Number"; }

// Return short summary of the node's purpose
std::string_view NumberNode::summary() const { return "A number"; }

// Perform processing
NodeConstants::ProcessResult NumberNode::process() { return NodeConstants::ProcessResult::Success; }
