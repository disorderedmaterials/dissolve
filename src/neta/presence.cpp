// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "neta/presence.h"
#include "classes/speciesatom.h"
#include "data/ff/atomtype.h"
#include <algorithm>

NETAPresenceNode::NETAPresenceNode(NETADefinition *parent, std::vector<Elements::Element> targetElements,
                                   std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes)
    : NETANode(parent, NETANode::NodeType::Presence)
{
    allowedElements_ = targetElements;
    allowedAtomTypes_ = targetAtomTypes;

    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::ComparisonOperator::EqualTo;
    nBondsValue_ = -1;
    nBondsValueOperator_ = NETANode::ComparisonOperator::EqualTo;
    nHydrogensValue_ = -1;
    nHydrogensValueOperator_ = NETANode::ComparisonOperator::EqualTo;
}

/*
 * Atom Targets
 */

// Add element target to node
bool NETAPresenceNode::addElementTarget(Elements::Element Z)
{
    allowedElements_.push_back(Z);

    return true;
}

// Add forcefield type target to node
bool NETAPresenceNode::addFFTypeTarget(const ForcefieldAtomType &ffType)
{
    allowedAtomTypes_.push_back(ffType);

    return true;
}

/*
 * Modifiers
 */

// Return enum options for NETACharacterModifiers
EnumOptions<NETAPresenceNode::NETACharacterModifier> NETAPresenceNode::modifiers()
{
    return EnumOptions<NETAPresenceNode::NETACharacterModifier>("CharacterModifier", {{NETACharacterModifier::NBonds, "nbonds"},
                                                                                      {NETACharacterModifier::NHydrogens, "nh"},
                                                                                      {NETACharacterModifier::Repeat, "n"}});
}

// Return whether the specified modifier is valid for this node
bool NETAPresenceNode::isValidModifier(std::string_view s) const { return modifiers().isValid(s); }

// Set value and comparator for specified modifier
bool NETAPresenceNode::setModifier(std::string_view modifier, ComparisonOperator op, int value)
{
    // Check that the supplied index is valid
    if (!modifiers().isValid(modifier))
        return Messenger::error("Invalid modifier '{}' passed to NETAPresenceNode.\n", modifier);

    switch (modifiers().enumeration(modifier))
    {
        case (NETAPresenceNode::NETACharacterModifier::NBonds):
            nBondsValue_ = value;
            nBondsValueOperator_ = op;
            break;
        case (NETAPresenceNode::NETACharacterModifier::NHydrogens):
            nHydrogensValue_ = value;
            nHydrogensValueOperator_ = op;
            break;
        case (NETAPresenceNode::NETACharacterModifier::Repeat):
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
int NETAPresenceNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &availableAtoms) const
{
    // We expect the passed SpeciesAtom 'i' to be nullptr, as our potential targets are held in availableAtoms (which we will
    // modify as appropriate)
    if (i != nullptr)
        return Messenger::error("Don't pass target atom to NETAPresenceNode - pass a list of possible atoms instead...\n");

    // Loop over the provided possible list of atoms
    auto nMatches = 0, totalScore = 0;
    std::vector<const SpeciesAtom *> matches;
    for (auto j : availableAtoms)
    {
        // Evaluate the atom against our elements
        int atomScore = NETANode::NoMatch;
        for (const auto Z : allowedElements_)
        {
            if (j->Z() != Z)
                continue;

            // Process branch definition via the base class, using a fresh path
            std::vector<const SpeciesAtom *> emptyPath;
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
                std::vector<const SpeciesAtom *> emptyPath;
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

        // Found a match, so increase the match count and score, and add the matched atom to our local list
        ++nMatches;
        totalScore += atomScore;
        matches.push_back(j);

        // Don't match more than we need to - check the repeatCount
        if (compareValues(nMatches, repeatCountOperator_, repeatCount_))
            break;
    }

    // Did we find the required number of matches in the provided list?
    if (!compareValues(nMatches, repeatCountOperator_, repeatCount_))
        return reverseLogic_ ? 1 : NETANode::NoMatch;

    // Remove any matched atoms from the original list
    for (auto j : matches)
        availableAtoms.erase(std::remove_if(availableAtoms.begin(), availableAtoms.end(),
                                            [&j](const auto &matchedAtom) { return matchedAtom == j; }),
                             availableAtoms.end());

    return reverseLogic_ ? NETANode::NoMatch : totalScore;
}
