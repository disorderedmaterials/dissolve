// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/hydrogenCount.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"

NETAHydrogenCountNode::NETAHydrogenCountNode(NETADefinition *parent, NETANode::ComparisonOperator op, std::optional<int> value)
    : NETANode(parent, NETANode::NodeType::HydrogenCount), value_(value), operator_(op)
{
}

/*
 * Bond Count
 */

// Set bond count requirements
void NETAHydrogenCountNode::set(ComparisonOperator op, int value)
{
    value_ = value;
    operator_ = op;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAHydrogenCountNode::score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const
{
    if (!value_)
        return NETANode::NoMatch;

    // Count number of hydrogens attached to this atom
    auto nH = std::count_if(i->bonds().begin(), i->bonds().end(),
                            [i](const SpeciesBond &bond) { return bond.partner(i)->Z() == Elements::H; });

    return compareValues(nH, operator_, *value_) ? 1 : NETANode::NoMatch;
}
