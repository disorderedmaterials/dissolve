// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/structure/speciesBase.h"

SpeciesBaseNode::SpeciesBaseNode(Graph *parentGraph) : Node(parentGraph)
{
    // Outputs
    addPointerOutput<const Species>("Species", "The species object", species_);

    // State data
    addStateData("Species", species_);
}

/*
 * Data
 */

// Return the species object
Species &SpeciesBaseNode::species() { return species_; }
const Species &SpeciesBaseNode::species() const { return species_; }
