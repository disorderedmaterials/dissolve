/*
    *** NETA Presence Node
    *** src/neta/presence.cpp
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

#include "neta/presence.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"

NETAPresenceNode::NETAPresenceNode(NETADefinition *parent, std::vector<Element *> targetElements,
                                   std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes,
                                   SpeciesBond::BondType bt)
    : NETANode(parent, NETANode::PresenceNode)
{
    allowedElements_ = targetElements;
    allowedAtomTypes_ = targetAtomTypes;

    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::EqualTo;
    nBondsValue_ = -1;
    nBondsValueOperator_ = NETANode::EqualTo;
    nHydrogensValue_ = -1;
    nHydrogensValueOperator_ = NETANode::EqualTo;
}

NETAPresenceNode::~NETAPresenceNode() {}

/*
 * Modifiers
 */

// Return enum options for NETACharacterModifiers
EnumOptions<NETAPresenceNode::NETACharacterModifier> NETAPresenceNode::modifiers()
{
    static EnumOptionsList ModifierOptions = EnumOptionsList()
                                             << EnumOption(NBondsModifier, "nbonds") << EnumOption(NHydrogensModifier, "nh")
                                             << EnumOption(RepeatCharacterModifier, "n");

    static EnumOptions<NETAPresenceNode::NETACharacterModifier> options("CharacterModifier", ModifierOptions);

    return options;
}

// Return whether the specified modifier is valid for this node
bool NETAPresenceNode::isValidModifier(std::string_view s) const { return (modifiers().isValid(s)); }

// Set value and comparator for specified modifier
bool NETAPresenceNode::setModifier(std::string_view modifier, ComparisonOperator op, int value)
{
    // Check that the supplied index is valid
    if (!modifiers().isValid(modifier))
        return Messenger::error("Invalid modifier '{}' passed to NETAPresenceNode.\n", modifier);

    switch (modifiers().enumeration(modifier))
    {
        case (NETAPresenceNode::NBondsModifier):
            nBondsValue_ = value;
            nBondsValueOperator_ = op;
            break;
        case (NETAPresenceNode::NHydrogensModifier):
            nHydrogensValue_ = value;
            nHydrogensValueOperator_ = op;
            break;
        case (NETAPresenceNode::RepeatCharacterModifier):
            repeatCount_ = value;
            repeatCountOperator_ = op;
            break;
        default:
            return Messenger::error("Don't know how to handle modifier '{}' in character node.\n", modifier);
    }

    return true;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAPresenceNode::score(const SpeciesAtom *i, RefList<const SpeciesAtom> &availableAtoms) const
{
    // We expect the passed SpeciesAtom 'i' to be nullptr, as our potential targets are held in availableAtoms (which we will
    // modify as appropriate)
    if (i != nullptr)
        return Messenger::error("Don't pass target atom to NETAPresenceNode - pass a list of possible atoms instead...\n");

    // Loop over the provided possible list of atoms
    auto nMatches = 0, totalScore = 0;
    RefList<const SpeciesAtom> matches;
    for (auto j : availableAtoms)
    {
        // Evaluate the atom against our elements
        int atomScore = NETANode::NoMatch;
        for (const auto *element : allowedElements_)
        {
            if (j->element() != element)
                continue;

            // Process branch definition via the base class, using a fresh path
            RefList<const SpeciesAtom> emptyPath;
            auto branchScore = NETANode::score(j, emptyPath);
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

                // Process branch definition via the base class, using an empty path
                RefList<const SpeciesAtom> emptyPath;
                auto branchScore = NETANode::score(j, emptyPath);
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
                return NETANode::NoMatch;

            ++atomScore;
        }

        // Found a match, so increase the match count and score, and add the matched atom to our local list
        ++nMatches;
        totalScore += atomScore;
        matches.append(j);
    }

    // Did we find the required number of matches in the provided list?
    if (!compareValues(nMatches, repeatCountOperator_, repeatCount_))
        return NETANode::NoMatch;

    // Remove any matched atoms from the original list
    for (auto j : matches)
        availableAtoms.remove(j);

    return totalScore;
}
