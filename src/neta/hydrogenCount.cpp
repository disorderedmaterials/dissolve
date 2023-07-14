// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "neta/hydrogenCount.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include "neta.h"

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
    auto newMatchPath = matchPath;
    if (!value_)
    {
        return NETANode::NoMatch;
    }

    // Count number of hydrogens attached to this atom, optionally appending them
    auto nH = 0;
    for (const auto& bond : i->bonds())
    {
        if (bond.get().partner(i)->Z() == Elements::H)
        {
            nH++;
            if (parent_->flags().isSet(NETADefinition::NETAFlags::MatchHydrogens))
            {
                Messenger::print("Flag is set!");
                newMatchPath.insert(bond.get().partner(i));
            }
        }
    }
    matchPath = newMatchPath;
    return compareValues(nH, operator_, *value_) ? 1 : NETANode::NoMatch;
}
