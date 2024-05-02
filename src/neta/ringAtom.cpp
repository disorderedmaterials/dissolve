// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/ringAtom.h"
#include "classes/speciesAtom.h"
#include "data/ff/atomType.h"
#include <algorithm>
#include <utility>

NETARingAtomNode::NETARingAtomNode(NETADefinition *parent, std::vector<Elements::Element> targetElements,
                                   std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes)
    : NETANode(parent, NETANode::NodeType::RingAtom)
{
    allowedElements_ = std::move(targetElements);
    allowedAtomTypes_ = std::move(targetAtomTypes);

    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::ComparisonOperator::EqualTo;
}

/*
 * Atom Targets
 */

// Add element target to node
bool NETARingAtomNode::addElementTarget(Elements::Element Z)
{
    allowedElements_.push_back(Z);

    return true;
}

// Add forcefield type target to node
bool NETARingAtomNode::addFFTypeTarget(const ForcefieldAtomType &ffType)
{
    allowedAtomTypes_.emplace_back(ffType);

    return true;
}

/*
 * Modifiers
 */

// Return enum options for NETACharacterModifiers
EnumOptions<NETARingAtomNode::NETACharacterModifier> NETARingAtomNode::modifiers()
{
    return EnumOptions<NETARingAtomNode::NETACharacterModifier>("CharacterModifier", {{NETACharacterModifier::Repeat, "n"}});
}

// Return whether the specified modifier is valid for this node
bool NETARingAtomNode::isValidModifier(std::string_view s) const { return modifiers().isValid(s); }

// Set value and comparator for specified modifier
bool NETARingAtomNode::setModifier(std::string_view modifier, ComparisonOperator op, int value)
{
    // Check that the supplied index is valid
    if (!modifiers().isValid(modifier))
        return Messenger::error("Invalid modifier '{}' passed to NETARingAtomNode.\n", modifier);

    switch (modifiers().enumeration(modifier))
    {
        case (NETARingAtomNode::NETACharacterModifier::Repeat):
            repeatCount_ = value;
            repeatCountOperator_ = op;
            break;
        default:
            return Messenger::error("Don't know how to handle modifier '{}' in character node.\n", modifier);
    }

    return true;
}

// Test repeat count value and modifier
bool NETARingAtomNode::validRepeatCount(int value) const { return compareValues(value, repeatCountOperator_, repeatCount_); }

/*
 * Scoring
 */

// Return whether we match the specified atom
int NETARingAtomNode::matches(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const
{
    // Evaluate the atom against our elements
    int atomScore = NETANode::NoMatch;
    if (std::find(allowedElements_.begin(), allowedElements_.end(), i->Z()) != allowedElements_.end())
        atomScore = 1;
    else
        for (const ForcefieldAtomType &atomType : allowedAtomTypes_)
        {
            // Check the element of the atom type against that of the neighbours
            if (i->Z() != atomType.Z())
                continue;

            // Evaluate the neighbour against the atom type
            atomScore = atomType.neta().score(i);
            if (atomScore != NETANode::NoMatch)
                break;
        }
    if (atomScore == NETANode::NoMatch)
        return reverseLogic_ ? 1 : NETANode::NoMatch;

    // Process branch definition via the base class, using an empty path
    NETAMatchedGroup newPath;
    auto branchScore = NETANode::sequenceScore(nodes_, i, newPath);
    if (branchScore == NETANode::NoMatch)
        return reverseLogic_ ? 1 : NETANode::NoMatch;

    // Track atoms matched in the neighbour branch
    matchPath.merge(newPath);

    // Add identifiers to the match data
    for (auto &id : identifiers())
        matchPath.addIdentifier(i, id);

    return reverseLogic_ ? NETANode::NoMatch : (atomScore + branchScore);
}

// Evaluate the node and return its score
int NETARingAtomNode::score(const SpeciesAtom *i, NETAMatchedGroup &availableAtoms) const
{
    throw(std::runtime_error("NETARingAtomNode was called via its score() function, but this should never be done.\n"));
}
