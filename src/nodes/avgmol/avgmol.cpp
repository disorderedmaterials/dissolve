// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/avgmol/avgmol.h"
#include "nodes/constants.h"

AvgMolNode::AvgMolNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput("Configuration", "Set target configuration for the module", targetConfiguration_);

    addOption("Site", "Target site about which to calculate average species geometry", targetSite_);
    addOption("ExportCoordinates", "Whether to save average coordinates to disk", exportFileAndFormat_);

    addPointerOutput<const Species>("Average Species", "The species with the average coordinates", averageSpecies_);
}

std::string_view AvgMolNode::type() const { return "AvgMol"; }

std::string_view AvgMolNode::summary() const { return "Calculate Average Molecule"; }
