// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/character.h"
#include "classes/speciesAtom.h"
#include "data/ff/atomType.h"
#include <algorithm>
#include <map>
#include <utility>

NETACharacterNode::NETACharacterNode(NETADefinition *parent, std::vector<Elements::Element> targetElements,
                                     std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes)
    : NETANode(parent, NETANode::NodeType::Connection)
{
    allowedElements_ = std::move(targetElements);
    allowedAtomTypes_ = std::move(targetAtomTypes);
}

/*
 * Atom Targets
 */

// Add element target to node
bool NETACharacterNode::addElementTarget(Elements::Element Z)
{
    allowedElements_.push_back(Z);

    return true;
}

// Add forcefield type target to node
bool NETACharacterNode::addFFTypeTarget(const ForcefieldAtomType &ffType)
{
    allowedAtomTypes_.emplace_back(ffType);

    return true;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETACharacterNode::score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const
{
    // Check element
    if (std::find(allowedElements_.begin(), allowedElements_.end(), i->Z()) != allowedElements_.end() && !reverseLogic_)
        return 1;

    // Check atom types
    for (const ForcefieldAtomType &atomType : allowedAtomTypes_)
    {
        // Check the element of the atom type against that of the neighbours
        if (i->Z() != atomType.Z())
            continue;

        // Evaluate the neighbour against the atom type
        auto typeScore = atomType.neta().score(i);
        if (typeScore != NETANode::NoMatch && !reverseLogic_)
            return typeScore;
    }

    return reverseLogic_ ? 1 : NETANode::NoMatch;
}
