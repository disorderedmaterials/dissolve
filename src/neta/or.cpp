// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "neta/or.h"
#include "neta/NETAErrorListeners.h"

NETAOrNode::NETAOrNode(NETADefinition *parent) : NETANode(parent, NETANode::NodeType::Or) {}

/*
 * Node Sequence
 */

// Set alternative node sequence
void NETAOrNode::setAlternativeNodes(NETASequence nodes) { altNodes_ = std::move(nodes); }

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAOrNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    auto score = sequenceScore(nodes_, i, matchPath);
    if (score != NETANode::NoMatch)
        return score;

    return sequenceScore(altNodes_, i, matchPath);
}
