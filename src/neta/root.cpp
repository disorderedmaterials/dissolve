// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "neta/root.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "data/elements.h"

NETARootNode::NETARootNode(NETADefinition *parent) : NETANode(parent, NETANode::NodeType::Root) {}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETARootNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    // Process branch definition via the base class
    return NETANode::sequenceScore(nodes_, i, matchPath);
}
