// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/forcefield.h"
#include "data/ff/library.h"
#include <toml11/toml.hpp>

ForcefieldNode::ForcefieldNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<Forcefield *>("Forcefield", "Source forcefield", recipe_.ff);
    addOutput("Recipe", "Recipe for the forcefield when applying to a species", recipe_);
}

/*
 * Definition
 */

std::string_view ForcefieldNode::type() const { return "Forcefield"; }

std::string_view ForcefieldNode::summary() const { return "Set up a forcefield to apply to a species"; }

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult ForcefieldNode::process() { return NodeConstants::ProcessResult::Success; }

/*
 * Serialisation
 */

// Serialise any hidden content
void ForcefieldNode::serialiseInternal(SerialisedValue &target) const { target["forcefield"] = recipe_.ff->name(); }

// Deserialise any hidden content
void ForcefieldNode::deserialiseInternal(const SerialisedValue &node)
{
    recipe_.ff = ForcefieldLibrary::forcefield(std::string_view(node.at("forcefield").as_string())).get();
}
