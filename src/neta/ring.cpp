// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "neta/ring.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"
#include <algorithm>

NETARingNode::NETARingNode(NETADefinition *parent) : NETANode(parent, NETANode::RingNode)
{
    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::GreaterThanEqualTo;
    sizeValue_ = -1;
    sizeValueOperator_ = NETANode::EqualTo;
}

NETARingNode::~NETARingNode() {}

/*
 * Modifiers
 */

// Return enum options for NETARingModifiers
EnumOptions<NETARingNode::NETARingModifier> NETARingNode::modifiers()
{
    static EnumOptionsList ModifierOptions = EnumOptionsList()
                                             << EnumOption(SizeModifier, "size") << EnumOption(RepeatRingModifier, "n");

    static EnumOptions<NETARingNode::NETARingModifier> options("RingModifier", ModifierOptions);

    return options;
}

// Return whether the specified modifier is valid for this node
bool NETARingNode::isValidModifier(std::string_view s) const { return modifiers().isValid(s); }

// Set value and comparator for specified modifier
bool NETARingNode::setModifier(std::string_view modifier, ComparisonOperator op, int value)
{
    // Check that the supplied index is valid
    if (!modifiers().isValid(modifier))
        return Messenger::error("Invalid modifier '{}' passed to NETARingNode.\n", modifier);

    switch (modifiers().enumeration(modifier))
    {
        case (NETARingNode::SizeModifier):
            sizeValue_ = value;
            sizeValueOperator_ = op;
            break;
        case (NETARingNode::RepeatRingModifier):
            repeatCount_ = value;
            repeatCountOperator_ = op;
            break;
        default:
            return Messenger::error("Don't know how to handle modifier '{}' in ring node.\n", modifier);
    }

    return true;
}

/*
 * Scoring
 */

// Locate rings in which the specified atom is involved
void NETARingNode::findRings(const SpeciesAtom *currentAtom, std::vector<SpeciesRing> &rings,
                             std::vector<const SpeciesAtom *> &path, const int minSize, const int maxSize) const
{
    // Check whether the path is already at the maximum size - if so, return immediately.
    if (path.size() == maxSize)
        return;

    // Add the current atom to the path
    path.push_back(currentAtom);

    // Loop over bonds to the atom
    const SpeciesAtom *j;
    for (const SpeciesBond &bond : currentAtom->bonds())
    {
        /*
         * Get the partner atom and compare to first atom in the current path.
         * If it is the currentAtom then we have found a cyclic route back to the originating atom.
         * If not, check whether the atom is already elsewhere in the path - if so, continue with the next bond.
         */
        j = bond.partner(currentAtom);
        if ((path.size() >= minSize) && (j == path.at(0)))
        {
            // Special case - if NotEqualTo was specified as the comparison operator, check that against the maximum
            // size
            if ((sizeValueOperator_ == NETANode::NotEqualTo) && (path.size() == maxSize))
                continue;

            // Add new ring
            rings.emplace_back(path);

            // Continue with the next bond
            continue;
        }
        else if (find(path.rbegin(), path.rend(), j) != path.rend())
            continue;

        // The current atom j is not in the path, so recurse
        findRings(j, rings, path, minSize, maxSize);
    }

    // Remove current atom from the path
    path.erase(path.end() - 1);
}

// Evaluate the node and return its score
int NETARingNode::score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const
{
    // Generate array of rings of specified size that the atom 'i' is present in
    std::vector<SpeciesRing> rings;
    std::vector<const SpeciesAtom *> ringPath;
    if (sizeValue_ == -1)
        findRings(i, rings, ringPath, 3, 6);
    else if (sizeValueOperator_ == NETANode::EqualTo)
        findRings(i, rings, ringPath, sizeValue_, sizeValue_);
    else if (sizeValueOperator_ == NETANode::LessThan)
        findRings(i, rings, ringPath, 3, sizeValue_ - 1);
    else if (sizeValueOperator_ == NETANode::LessThanEqualTo)
        findRings(i, rings, ringPath, 3, sizeValue_);
    else if (sizeValueOperator_ == NETANode::GreaterThan)
        findRings(i, rings, ringPath, sizeValue_ + 1, 99);
    else if (sizeValueOperator_ == NETANode::GreaterThanEqualTo)
        findRings(i, rings, ringPath, sizeValue_, 99);
    else
        findRings(i, rings, ringPath, 3, 99);

    // Prune rings for duplicates
    for (auto it = rings.begin(); it != rings.end(); ++it)
        rings.erase(std::remove_if(it + 1, rings.end(), [&it](const auto &otherIt) { return *it == otherIt; }));

    // Loop over rings
    auto nMatches = 0, totalScore = 0;
    int nodeScore = NETANode::NoMatch;
    for (auto ring : rings)
    {
        // Check through atoms in the ring - either in order or not - to see if the ring matches
        // Disordered search - try to match the branch definition against this ring, in any order (provide a copy of all
        // atoms in the ring at once)

        auto ringScore = 0;
        std::vector<const SpeciesAtom *> ringAtoms = ring.atoms();
        for (auto node : branch_)
        {
            nodeScore = node->score(nullptr, ringAtoms);
            if (nodeScore == NETANode::NoMatch)
            {
                ringScore = NETANode::NoMatch;
                break;
            }

            // Match found
            ringScore += nodeScore;
        }

        // If we didn't find a match for the ring, continue to the next one
        if (ringScore == NETANode::NoMatch)
            continue;

        ++nMatches;

        // Increase the total score - ringScore (contained atoms) + 1 (for the ring) + 1 (for the size, if specified)
        totalScore += ringScore + 1;
        if (sizeValue_ != -1)
            ++totalScore;

        // Don't match more than we need to - check the repeatCount
        if (compareValues(nMatches, repeatCountOperator_, repeatCount_))
            break;
    }

    // Did we find the required number of ring matches?
    if (!compareValues(nMatches, repeatCountOperator_, repeatCount_))
        return reverseLogic_ ? 1 : NETANode::NoMatch;

    return reverseLogic_ ? NETANode::NoMatch : totalScore;
}
