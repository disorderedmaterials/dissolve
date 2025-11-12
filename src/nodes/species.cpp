// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "species.h"
#include "classes/coreData.h"
#include "nodes/constants.h"
#include <sstream>
#include <toml11/toml.hpp>
#include <toml11/toml/parser.hpp>

SpeciesNode::SpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption("Definition", "TOML that defines the species", definition_);
    addPointerOutput<const Species>("Species", "Created species", species_);
}

std::string_view SpeciesNode::type() const { return "Species"; }

std::string_view SpeciesNode::summary() const { return "Produce a species"; }

NodeConstants::ProcessResult SpeciesNode::process()
{
    CoreData coreData;
    std::istringstream stream(definition_);
    SerialisedValue node = toml::parse(stream);
    species_.deserialise(node, coreData);
    atomTypes_ = coreData.atomTypes();
    return NodeConstants::ProcessResult::Success;
}
