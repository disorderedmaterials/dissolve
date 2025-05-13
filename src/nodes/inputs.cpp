// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/inputs.h"

InputsNode::InputsNode(Graph *parentGraph, NodeParameterMap &inputs) : Node(parentGraph), inputs_(inputs) {}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view InputsNode::type() const { return "Inputs"; }

// Return short summary of the node's purpose
std::string_view InputsNode::summary() const { return "Input parameters"; }
