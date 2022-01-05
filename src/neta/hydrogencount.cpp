// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "neta/hydrogencount.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"

NETAHydrogenCountNode::NETAHydrogenCountNode(NETADefinition *parent)
    : NETANode(parent, NETANode::NodeType::HydrogenCount), value_(-1), operator_(NETANode::ComparisonOperator::EqualTo)
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
int NETAHydrogenCountNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    // Count number of hydrogens attached to this atom
    auto nH = std::count_if(i->bonds().begin(), i->bonds().end(),
                            [i](const SpeciesBond &bond) { return bond.partner(i)->Z() == Elements::H; });

    return compareValues(nH, operator_, value_) ? 1 : NETANode::NoMatch;
}
