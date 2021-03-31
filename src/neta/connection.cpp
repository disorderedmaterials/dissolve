// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "neta/connection.h"
#include "classes/speciesatom.h"
#include "data/ff/atomtype.h"
#include <algorithm>
#include <map>

NETAConnectionNode::NETAConnectionNode(NETADefinition *parent, std::vector<Elements::Element> targetElements,
                                       std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes,
                                       SpeciesBond::BondType bt)
    : NETANode(parent, NETANode::NodeType::Connection)
{
    allowedElements_ = targetElements;
    allowedAtomTypes_ = targetAtomTypes;
    bondType_ = bt;
    geometry_ = SpeciesAtom::AtomGeometry::Unknown;
    geometryOperator_ = NETANode::ComparisonOperator::EqualTo;

    // Modifiers
    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::ComparisonOperator::GreaterThanEqualTo;
    nBondsValue_ = -1;
    nBondsValueOperator_ = NETANode::ComparisonOperator::EqualTo;
    nHydrogensValue_ = -1;
    nHydrogensValueOperator_ = NETANode::ComparisonOperator::EqualTo;

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
                                                                   {{NETAConnectionModifier::NBonds, "nbonds"},
                                                                    {NETAConnectionModifier::NHydrogens, "nh"},
                                                                    {NETAConnectionModifier::Repeat, "n"}});
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
        case (NETAConnectionNode::NETAConnectionModifier::NBonds):
            nBondsValue_ = value;
            nBondsValueOperator_ = op;
            break;
        case (NETAConnectionNode::NETAConnectionModifier::NHydrogens):
            nHydrogensValue_ = value;
            nHydrogensValueOperator_ = op;
            break;
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
 * Options
 */

// Return enum options for NETARootOptions
EnumOptions<NETAConnectionNode::NETAConnectionOption> NETAConnectionNode::options()
{
    return EnumOptions<NETAConnectionNode::NETAConnectionOption>("ConnectionOption",
                                                                 {{NETAConnectionOption::Geometry, "geometry"}});
}

// Return whether the specified option is valid for this node
bool NETAConnectionNode::isValidOption(std::string_view s) const { return options().isValid(s); }

// Set value and comparator for specified option
bool NETAConnectionNode::setOption(std::string_view option, ComparisonOperator op, std::string_view value)
{
    // Check that the supplied option is valid
    if (!options().isValid(option))
        return Messenger::error("Invalid option '{}' passed to NETARootNode.\n", option);

    switch (options().enumeration(option))
    {
        case (NETAConnectionNode::NETAConnectionOption::Geometry):
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
int NETAConnectionNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    // Get directly connected atoms about 'i', excluding any that have already been matched
    std::map<const SpeciesAtom *, int> neighbours;
    for (const SpeciesBond &bond : i->bonds())
    {
        const auto *partner = bond.partner(i);

        // Search for this species atom in the current match path
        auto atomIt =
            std::find_if(matchPath.begin(), matchPath.end(), [partner](const auto spAtom) { return spAtom == partner; });
        if (atomIt == matchPath.end())
            neighbours.emplace(partner, NETANode::NoMatch);
        else if (atomIt == matchPath.begin() && allowRootMatch_)
            neighbours.emplace(partner, NETANode::NoMatch);
    }

    // Loop over neighbour atoms
    auto nMatches = 0;
    for (auto &nbr : neighbours)
    {
        auto *j = nbr.first;

        // Evaluate the neighbour against our elements
        int atomScore = NETANode::NoMatch;
        for (const auto Z : allowedElements_)
        {
            if (j->Z() != Z)
                continue;

            // Process branch definition via the base class, using a copy of the current match path
            auto branchMatchPath = matchPath;

            // Add ourselves to the match path so we can't backtrack
            branchMatchPath.push_back(i);

            auto branchScore = NETANode::score(j, branchMatchPath);
            if (branchScore == NETANode::NoMatch)
                continue;

            // Create total score (element match plus branch score)
            atomScore = 1 + branchScore;

            // Now have a match, so break out of the loop
            break;
        }
        if (atomScore == NETANode::NoMatch)
            for (const ForcefieldAtomType &atomType : allowedAtomTypes_)
            {
                // Check the element of the atom type against that of the neighbours
                if (j->Z() != atomType.Z())
                    continue;

                // Evaluate the neighbour against the atom type
                auto typeScore = atomType.neta().score(j);
                if (typeScore == NETANode::NoMatch)
                    continue;

                // Process branch definition via the base class, using a copy of the current match path
                auto branchMatchPath = matchPath;

                // Add ourselves to the match path so we can't backtrack
                branchMatchPath.push_back(i);

                auto branchScore = NETANode::score(j, branchMatchPath);
                if (branchScore == NETANode::NoMatch)
                    continue;

                // Create total score
                atomScore = typeScore + branchScore;

                // Now have a match, so break out of the loop
                break;
            }

        // Did we match the atom?
        if (atomScore == NETANode::NoMatch)
            continue;

        // Check any specified modifier values
        if (nBondsValue_ >= 0)
        {
            if (!compareValues(j->nBonds(), nBondsValueOperator_, nBondsValue_))
                continue;

            ++atomScore;
        }
        if (nHydrogensValue_ >= 0)
        {
            // Count number of hydrogens attached to this atom
            auto nH = std::count_if(j->bonds().begin(), j->bonds().end(),
                                    [j](const SpeciesBond &bond) { return bond.partner(j)->Z() == Elements::H; });
            if (!compareValues(nH, nHydrogensValueOperator_, nHydrogensValue_))
                continue;

            ++atomScore;
        }

        // Found a match, so increase the match count and store the score
        ++nMatches;
        nbr.second = atomScore;

        // Exit early in the case of GreaterThan GreaterThanEqualTo logic
        if ((repeatCountOperator_ == NETANode::ComparisonOperator::GreaterThan ||
             repeatCountOperator_ == NETANode::ComparisonOperator::GreaterThanEqualTo) &&
            compareValues(nMatches, repeatCountOperator_, repeatCount_))
            break;
    }

    // Did we find the required number of matches in the neighbour list?
    if (!compareValues(nMatches, repeatCountOperator_, repeatCount_))
        return reverseLogic_ ? 1 : NETANode::NoMatch;

    // Generate total score and add matched atoms to the path
    auto totalScore = 0;
    for (auto nbr : neighbours)
    {
        if (nbr.second == NETANode::NoMatch)
            continue;

        totalScore += nbr.second;
        matchPath.push_back(nbr.first);
    }

    return reverseLogic_ ? NETANode::NoMatch : totalScore;
}
