// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "neta/bondcount.h"
#include "classes/speciesatom.h"

NETABondCountNode::NETABondCountNode(NETADefinition *parent, NETANode::ComparisonOperator op, std::optional<int> value)
    : NETANode(parent, NETANode::NodeType::BondCount), value_(value), operator_(op)
{
}

/*
 * Bond Count
 */

// Set bond count requirements
void NETABondCountNode::set(ComparisonOperator op, int value)
{
    value_ = value;
    operator_ = op;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETABondCountNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    if (!value_)
        return NETANode::NoMatch;

    return compareValues(i->nBonds(), operator_, *value_) ? 1 : NETANode::NoMatch;
}
