// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importSpecies.h"

ImportSpeciesNode::ImportSpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addPointerOutput<const Species>("Species", "Created species", species_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportSpeciesNode::type() const { return "ImportSpecies"; }

// Return short summary of the node's purpose
std::string_view ImportSpeciesNode::summary() const { return "Import species data from TOML"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportSpeciesNode::process()
{
    auto filePath = filePath_.string();

    message("Reading species data from '{}'...\n", filePath);

    // Parse the file
    species_.load(filePath);

    return NodeConstants::ProcessResult::Success;
}
