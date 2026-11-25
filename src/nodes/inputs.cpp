// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/inputs.h"
#include "nodes/iterableGraph.h"

InputsNode::InputsNode(Graph *parentGraph) : Node(parentGraph) {}

/*
 * Definition
 */

// Return type of the node
std::string_view InputsNode::type() const { return "Inputs"; }

// Return short summary of the node's purpose
std::string_view InputsNode::summary() const { return "Maps graph inputs to local outputs"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult InputsNode::process() { return NodeConstants::ProcessResult::Success; }

/*
 * Serialisation
 */

// Is it appropriate to serialise this node?
bool InputsNode::shouldSerialise() const { return false; }
