// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/setCoordinates.h"

SetCoordinatesNode::SetCoordinatesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration to modify", configuration_);
    addInput("Structure", "Structure to apply", structure_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", configuration_);
}

std::string_view SetCoordinatesNode::type() const { return "SetCoordinates"; }

std::string_view SetCoordinatesNode::summary() const { return "Set coordinates of a configuration from a source structure."; }

NodeConstants::ProcessResult SetCoordinatesNode::process()
{
    // Check sizes
    if (configuration_->nAtoms() != structure_.nAtoms())
        return error("Mismatch between sizes of configuration and source structure data ({} vs {} atoms).\n",
                     configuration_->nAtoms(), structure_.nAtoms());

    // Copy atom positions
    for (auto &&[cfgAtom, structureAtom] : zip(configuration_->atoms(), structure_.atoms()))
        cfgAtom.setR(structureAtom->r());

    return NodeConstants::ProcessResult::Success;
}
