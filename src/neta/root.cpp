// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "neta/root.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "data/elements.h"

NETARootNode::NETARootNode(NETADefinition *parent) : NETANode(parent, NETANode::NodeType::Root)
{
    geometry_ = SpeciesAtom::AtomGeometry::Unknown;
    geometryOperator_ = NETANode::ComparisonOperator::EqualTo;
}

/*
 * Options
 */

// Return enum options for NETARootOptions
EnumOptions<NETARootNode::NETARootOption> NETARootNode::options()
{
    return EnumOptions<NETARootNode::NETARootOption>("RootOption", {{NETARootOption::Geometry, "geometry"}});
}

// Return whether the specified option is valid for this node
bool NETARootNode::isValidOption(std::string_view s) const { return options().isValid(s); }

// Set value and comparator for specified option
bool NETARootNode::setOption(std::string_view option, ComparisonOperator op, std::string_view value)
{
    // Check that the supplied option is valid
    if (!options().isValid(option))
        return Messenger::error("Invalid option '{}' passed to NETARootNode.\n", option);

    switch (options().enumeration(option))
    {
        case (NETARootNode::NETARootOption::Geometry):
            // Check that the value is a valid AtomGeometry
            if (SpeciesAtom::geometries().isValid(value))
                geometry_ = SpeciesAtom::geometries().enumeration(value);
            else
                return SpeciesAtom::geometries().errorAndPrintValid(value);
            geometryOperator_ = op;
            break;
        default:
            return Messenger::error("Don't know how to handle option '{}' in root node.\n", option);
    }

    return true;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETARootNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    auto totalScore = 0;

    if (geometry_ != SpeciesAtom::AtomGeometry::Unknown)
    {
        auto result = i->isGeometry(geometry_);
        if ((geometryOperator_ == NETANode::ComparisonOperator::EqualTo && !result) ||
            (geometryOperator_ == NETANode::ComparisonOperator::NotEqualTo && result))
            return NETANode::NoMatch;

        ++totalScore;
    }

    // Process branch definition via the base class
    auto branchScore = NETANode::sequenceScore(nodes_, i, matchPath);
    if (branchScore == NETANode::NoMatch)
        return NETANode::NoMatch;

    totalScore += branchScore;

    // If the score is still zero, return NoMatch
    return totalScore == 0 ? NETANode::NoMatch : totalScore;
}
