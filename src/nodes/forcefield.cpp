// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/forcefield.h"
#include "data/ff/library.h"

ForcefieldNode::ForcefieldNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption("Forcefield", "Source forcefield", recipe_.ff);
    addOutput("Recipe", "Recipe for the forcefield when applying to a species", recipe_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ForcefieldNode::type() const { return "Forcefield"; }

// Return short summary of the node's purpose
std::string_view ForcefieldNode::summary() const { return "Set up a forcefield to apply to a species"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ForcefieldNode::process() { return NodeConstants::ProcessResult::Success; }
