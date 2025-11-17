// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "species.h"
#include "nodes/constants.h"
#include <toml11/toml.hpp>
#include <toml11/toml/parser.hpp>

SpeciesNode::SpeciesNode(Graph *parentGraph, std::unique_ptr<Species> &&species)
    : Node(parentGraph), species_(std::move(species))
{
    addPointerOutput<const Species>("Species", "Created species", *species_);
}

std::string_view SpeciesNode::type() const { return "Species"; }

std::string_view SpeciesNode::summary() const { return "Produce a species"; }

NodeConstants::ProcessResult SpeciesNode::process() { return NodeConstants::ProcessResult::Unchanged; }
