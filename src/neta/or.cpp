// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "neta/or.h"
#include "neta/NETAErrorListeners.h"

NETAOrNode::NETAOrNode(NETADefinition *parent) : NETANode(parent, NETANode::NodeType::Or) {}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAOrNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    if (branch_.size() != 2)
        throw NETAExceptions::NETAInternalErrorException("Logical 'OR' NETA node must have exactly two nodes in its branch.");

    auto score = branch_[0]->score(i, matchPath);
    if (score == NETANode::NoMatch)
        score = branch_[1]->score(i, matchPath);

    return score;
}
