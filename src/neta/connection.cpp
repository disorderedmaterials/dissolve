/*
    *** NETA Connection Node
    *** src/neta/connection.cpp
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "neta/connection.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"
#include "templates/dynamicarray.h"
#include "templates/refdatalist.h"

NETAConnectionNode::NETAConnectionNode(NETADefinition *parent, std::vector<Element *> targetElements,
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
bool NETAConnectionNode::isValidModifier(std::string_view s) const { return (modifiers().isValid(s)); }

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
bool NETAConnectionNode::isValidFlag(std::string_view s) const { return (flags().isValid(s)); }

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
int NETAConnectionNode::score(const SpeciesAtom *i, RefList<const SpeciesAtom> &matchPath) const
{
    // Get directly connected atoms about 'i', excluding any that have already been matched
    RefDataList<const SpeciesAtom, int> neighbours;
    for (const SpeciesBond &bond : i->bonds())
    {
        const auto *partner = bond.partner(i);
        if (partner == matchPath.firstItem())
        {
            // We may allow the path's root atom to be matched again, if the allowRootMatch_ is set...
            if (!allowRootMatch_)
                continue;
        }
        else if (matchPath.contains(partner))
            continue;
        neighbours.append(partner, NETANode::NoMatch);
    }

    // Loop over neighbour atoms
    auto nMatches = 0;
    RefDataListIterator<const SpeciesAtom, int> neighbourIterator(neighbours);
    while (const SpeciesAtom *j = neighbourIterator.iterate())
    {
        // Evaluate the neighbour against our elements
        int atomScore = NETANode::NoMatch;
        for (const auto *element : allowedElements_)
        {
            if (j->element() != element)
                continue;

            // Process branch definition via the base class, using a copy of the current match path
            RefList<const SpeciesAtom> branchMatchPath = matchPath;

            // Add ourselves to the match path so we can't backtrack
            branchMatchPath.append(i);

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
                RefList<const SpeciesAtom> branchMatchPath = matchPath;
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
        matchPath.append(j);
    }

    return totalScore;
}
