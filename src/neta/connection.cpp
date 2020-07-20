// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "neta/connection.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"
#include "templates/dynamicarray.h"
#include "templates/refdatalist.h"
#include <algorithm>

NETAConnectionNode::NETAConnectionNode(NETADefinition *parent,
                                       std::vector<std::reference_wrapper<const Element>> targetElements,
                                       std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes,
                                       SpeciesBond::BondType bt)
    : NETANode(parent, NETANode::ConnectionNode)
{
    allowedElements_ = targetElements;
    allowedAtomTypes_ = targetAtomTypes;
    bondType_ = bt;

    // Modifiers
    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::GreaterThanEqualTo;
    nBondsValue_ = -1;
    nBondsValueOperator_ = NETANode::EqualTo;
    nHydrogensValue_ = -1;
    nHydrogensValueOperator_ = NETANode::EqualTo;

    // Flags
    allowRootMatch_ = false;
}

NETAConnectionNode::~NETAConnectionNode() {}

/*
 * Atom Targets
 */

// Add element target to node
bool NETAConnectionNode::addElementTarget(const Element &el)
{
    allowedElements_.push_back(el);

    return true;
}

// Add forcefield type target to node
bool NETAConnectionNode::addFFTypeTarget(const ForcefieldAtomType &ffType)
{
    allowedAtomTypes_.push_back(ffType);

    return true;
}

/*
 * Modifiers
 */

// Return enum options for NETAConnectionModifiers
EnumOptions<NETAConnectionNode::NETAConnectionModifier> NETAConnectionNode::modifiers()
{
    static EnumOptionsList ModifierOptions = EnumOptionsList()
                                             << EnumOption(NBondsModifier, "nbonds") << EnumOption(NHydrogensModifier, "nh")
                                             << EnumOption(RepeatConnectionModifier, "n");

    static EnumOptions<NETAConnectionNode::NETAConnectionModifier> options("ConnectionModifier", ModifierOptions);

    return options;
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
        case (NETAConnectionNode::NBondsModifier):
            nBondsValue_ = value;
            nBondsValueOperator_ = op;
            break;
        case (NETAConnectionNode::NHydrogensModifier):
            nHydrogensValue_ = value;
            nHydrogensValueOperator_ = op;
            break;
        case (NETAConnectionNode::RepeatConnectionModifier):
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
    static EnumOptionsList FlagOptions = EnumOptionsList() << EnumOption(RootFlag, "root");

    static EnumOptions<NETAConnectionNode::NETAConnectionFlag> options("ConnectionFlag", FlagOptions);

    return options;
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
        case (NETAConnectionNode::RootFlag):
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
int NETAConnectionNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom*> &matchPath) const
{
    // Get directly connected atoms about 'i', excluding any that have already been matched
    RefDataList<const SpeciesAtom, int> neighbours;
    for (const SpeciesBond &bond : i->bonds())
    {
        const auto *partner = bond.partner(i);

        // Search for this species atom in the current match path
        auto atomIt = std::find_if(matchPath.begin(), matchPath.end(), [partner](const auto spAtom) { return spAtom == partner; });
        if (atomIt == matchPath.end()) neighbours.append(partner, NETANode::NoMatch);
        else if (atomIt == matchPath.begin() && allowRootMatch_) neighbours.append(partner, NETANode::NoMatch);
    }

    // Loop over neighbour atoms
    auto nMatches = 0;
    RefDataListIterator<const SpeciesAtom, int> neighbourIterator(neighbours);
    while (const SpeciesAtom *j = neighbourIterator.iterate())
    {
        // Evaluate the neighbour against our elements
        int atomScore = NETANode::NoMatch;
        for (const auto &element : allowedElements_)
        {
            if (j->element() != &element.get())
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
                // Evaluate the neighbour against the atom type
                auto typeScore = atomType.neta().score(j);
                if (typeScore == NETANode::NoMatch)
                    continue;

                // Process branch definition via the base class, using a copy of the current match path
                auto branchMatchPath = matchPath;
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
                return NETANode::NoMatch;

            ++atomScore;
        }
        if (nHydrogensValue_ >= 0)
        {
            // Count number of hydrogens attached to this atom
            auto nH = std::count_if(j->bonds().begin(), j->bonds().end(),
                                    [j](const SpeciesBond &bond) { return bond.partner(j)->element()->Z() == ELEMENT_H; });
            if (!compareValues(nH, nHydrogensValueOperator_, nHydrogensValue_))
                continue;

            ++atomScore;
        }

        // Found a match, so increase the match count and store the score
        ++nMatches;
        neighbourIterator.currentData() = atomScore;

        // Exit early in the case of GreaterThan GreaterThanEqualTo logic
        if ((repeatCountOperator_ == NETANode::GreaterThan || repeatCountOperator_ == NETANode::GreaterThanEqualTo) &&
            compareValues(nMatches, repeatCountOperator_, repeatCount_))
            break;
    }

    // Did we find the required number of matches in the neighbour list?
    if (!compareValues(nMatches, repeatCountOperator_, repeatCount_))
        return NETANode::NoMatch;

    // Generate total score and add matched atoms to the path
    auto totalScore = 0;
    neighbourIterator.restart();
    while (const SpeciesAtom *j = neighbourIterator.iterate())
    {
        if (neighbourIterator.currentData() == NETANode::NoMatch)
            continue;

        totalScore += neighbourIterator.currentData();
        matchPath.push_back(j);
    }

    return totalScore;
}
