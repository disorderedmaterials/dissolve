// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/connection.h"
#include "classes/speciesAtom.h"
#include "data/ff/atomType.h"
#include <algorithm>
#include <map>
#include <utility>

NETAConnectionNode::NETAConnectionNode(NETADefinition *parent, std::vector<Elements::Element> targetElements,
                                       std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes,
                                       SpeciesBond::BondType bt)
    : NETANode(parent, NETANode::NodeType::Connection)
{
    allowedElements_ = std::move(targetElements);
    allowedAtomTypes_ = std::move(targetAtomTypes);
    bondType_ = bt;

    // Modifiers
    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::ComparisonOperator::GreaterThanEqualTo;

    // Flags
    allowRootMatch_ = false;
}

/*
 * Atom Targets
 */

// Add element target to node
bool NETAConnectionNode::addElementTarget(Elements::Element Z)
{
    allowedElements_.push_back(Z);

    return true;
}

// Add forcefield type target to node
bool NETAConnectionNode::addFFTypeTarget(const ForcefieldAtomType &ffType)
{
    allowedAtomTypes_.emplace_back(ffType);

    return true;
}

/*
 * Modifiers
 */

// Return enum options for NETAConnectionModifiers
EnumOptions<NETAConnectionNode::NETAConnectionModifier> NETAConnectionNode::modifiers()
{
    return EnumOptions<NETAConnectionNode::NETAConnectionModifier>("ConnectionModifier",
                                                                   {{NETAConnectionModifier::Repeat, "n"}});
}

// Return whether the specified modifier is valid for this node
bool NETAConnectionNode::isValidModifier(std::string_view s) const { return modifiers().isValid(s); }

// Set value and comparator for specified modifier
bool NETAConnectionNode::setModifier(std::string_view modifier, ComparisonOperator op, int value)
{
    // Check that the supplied index is valid
    if (!modifiers().isValid(modifier))
        return Messenger::error("Invalid modifier '{}' passed to NETAConnectionNode.\n", modifier);

    switch (modifiers().enumeration(modifier))
    {
        case (NETAConnectionNode::NETAConnectionModifier::Repeat):
            repeatCount_ = value;
            repeatCountOperator_ = op;
            break;
        default:
            return Messenger::error("Don't know how to handle modifier '{}' in connection node.\n", modifier);
    }

    return true;
}

/*
 * Flags
 */

// Return enum options for NETAConnectionFlags
EnumOptions<NETAConnectionNode::NETAConnectionFlag> NETAConnectionNode::flags()
{
    return EnumOptions<NETAConnectionNode::NETAConnectionFlag>("ConnectionFlag", {{NETAConnectionFlag::Root, "root"}});
}

// Return whether the specified flag is valid for this node
bool NETAConnectionNode::isValidFlag(std::string_view s) const { return flags().isValid(s); }

// Set specified flag
bool NETAConnectionNode::setFlag(std::string_view flag, bool state)
{
    // Check that the supplied index is valid
    if (!flags().isValid(flag))
        return Messenger::error("Invalid flag '{}' passed to NETAConnectionNode.\n", flag);

    switch (flags().enumeration(flag))
    {
        case (NETAConnectionNode::NETAConnectionFlag::Root):
            allowRootMatch_ = state;
            break;
        default:
            return Messenger::error("Don't know how to handle flag '{}' in connection node.\n", flag);
    }

    return true;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAConnectionNode::score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const
{
    // Get directly connected atoms about 'i', excluding any that have already been matched
    std::map<const SpeciesAtom *, std::pair<int, NETAMatchedGroup>> neighbours;
    for (const SpeciesBond &bond : i->bonds())
    {
        const auto *partner = bond.partner(i);

        // Search for this atom in the current match path
        if (!matchPath.contains(partner) || (allowRootMatch_ && matchPath.isRoot(partner)))
            neighbours.emplace(partner, std::pair<int, NETAMatchedGroup>(NETANode::NoMatch, matchPath));
    }

    // Loop over neighbour atoms
    auto nMatches = 0;
    for (auto &nbr : neighbours)
    {
        auto *j = nbr.first;
        auto &&[jScore, jMatchPath] = nbr.second;

        // Evaluate the neighbour against our elements
        jScore = NETANode::NoMatch;
        if (std::find(allowedElements_.begin(), allowedElements_.end(), j->Z()) != allowedElements_.end())
            jScore = 1;
        else
            for (const ForcefieldAtomType &atomType : allowedAtomTypes_)
            {
                // Check the element of the atom type against that of the neighbours
                if (j->Z() != atomType.Z())
                    continue;

                // Evaluate the neighbour against the atom type
                jScore = atomType.neta().score(j);
                if (jScore != NETANode::NoMatch)
                    break;
            }

        // Did we match an element or atom type?
        if (jScore == NETANode::NoMatch)
            continue;

        // Evaluate any branch definition
        auto branchScore = NETANode::sequenceScore(nodes_, j, jMatchPath);
        if (branchScore == NETANode::NoMatch)
            continue;

        // Found a match, so increase the match count and store the score
        jScore += branchScore;
        ++nMatches;

        // Exit early in the case of GreaterThan GreaterThanEqualTo logic
        if ((repeatCountOperator_ == NETANode::ComparisonOperator::GreaterThan ||
             repeatCountOperator_ == NETANode::ComparisonOperator::GreaterThanEqualTo) &&
            compareValues(nMatches, repeatCountOperator_, repeatCount_))
            break;
    }

    // Did we find the required number of matches in the neighbour list?
    if (!compareValues(nMatches, repeatCountOperator_, repeatCount_))
        return reverseLogic_ ? 1 : NETANode::NoMatch;

    // Successfully matched all connectivity - if reverseLogic_ is enabled, return now
    if (reverseLogic_)
        return NETANode::NoMatch;

    // Generate total score and add matched atoms to the path
    auto totalScore = 0;
    for (auto &nbr : neighbours)
    {
        auto &&[jScore, jMatchPath] = nbr.second;
        if (jScore == NETANode::NoMatch)
            continue;

        totalScore += jScore;

        // Track atoms matched in the neighbour branch
        matchPath.merge(jMatchPath);

        // Add identifiers to the match data
        for (auto &id : identifiers())
            matchPath.addIdentifier(nbr.first, id);
    }

    return totalScore;
}
