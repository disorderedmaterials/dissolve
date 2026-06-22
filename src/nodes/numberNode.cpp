// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/numberNode.h"

NumberNode::NumberNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<Number>("X", "A number", x_);
    addOutput<Number>("X", "A number", x_);
}

/*
 * Definition
 */

// Return type of the node
// Return type of the node
std::string_view NumberNode::type() const { return "Number"; }

// Return short summary of the node's purpose
// Return short summary of the node's purpose
std::string_view NumberNode::summary() const { return "A number"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult NumberNode::process() { return NodeConstants::ProcessResult::Success; }
