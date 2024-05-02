// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/ring.h"
#include "classes/speciesAtom.h"
#include "data/ff/atomType.h"
#include "neta/ringAtom.h"
#include <algorithm>
#include <numeric>

NETARingNode::NETARingNode(NETADefinition *parent) : NETANode(parent, NETANode::NodeType::Ring)
{
    repeatCount_ = 1;
    repeatCountOperator_ = NETANode::ComparisonOperator::GreaterThanEqualTo;
    sizeValueOperator_ = NETANode::ComparisonOperator::EqualTo;
}

/*
 * Modifiers
 */

// Return enum options for NETARingModifiers
EnumOptions<NETARingNode::NETARingModifier> NETARingNode::modifiers()
{
    return EnumOptions<NETARingNode::NETARingModifier>("RingModifier",
                                                       {{NETARingModifier::Size, "size"}, {NETARingModifier::Repeat, "n"}});
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
        case (NETARingNode::NETARingModifier::Size):
            sizeValue_ = value;
            sizeValueOperator_ = op;
            break;
        case (NETARingNode::NETARingModifier::Repeat):
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
            if ((sizeValueOperator_ == NETANode::ComparisonOperator::NotEqualTo) && (path.size() == maxSize))
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
int NETARingNode::score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const
{
    // Generate array of rings of specified size that the atom 'i' is present in
    std::vector<SpeciesRing> rings;
    std::vector<const SpeciesAtom *> ringPath;
    if (!sizeValue_)
        findRings(i, rings, ringPath, 3, 6);
    else if (sizeValueOperator_ == NETANode::ComparisonOperator::EqualTo)
        findRings(i, rings, ringPath, *sizeValue_, *sizeValue_);
    else if (sizeValueOperator_ == NETANode::ComparisonOperator::LessThan)
        findRings(i, rings, ringPath, 3, *sizeValue_ - 1);
    else if (sizeValueOperator_ == NETANode::ComparisonOperator::LessThanEqualTo)
        findRings(i, rings, ringPath, 3, *sizeValue_);
    else if (sizeValueOperator_ == NETANode::ComparisonOperator::GreaterThan)
        findRings(i, rings, ringPath, *sizeValue_ + 1, 99);
    else if (sizeValueOperator_ == NETANode::ComparisonOperator::GreaterThanEqualTo)
        findRings(i, rings, ringPath, *sizeValue_, 99);
    else
        findRings(i, rings, ringPath, 3, 99);

    // Prune rings for duplicates
    rings.erase(std::unique(rings.begin(), rings.end()), rings.end());

    std::map<const SpeciesRing *, std::pair<int, NETAMatchedGroup>> matchedRings;

    // Loop over rings
    for (const auto &ring : rings)
    {
        // Check through atoms in the ring in the order specified
        for (auto ringAtomIt = ring.atoms().begin(); ringAtomIt != ring.atoms().end(); ++ringAtomIt)
        {
            // Copy the iterator to use for forward and backward traversal around the ring
            auto forwardIt = ringAtomIt, backwardIt = ringAtomIt;

            std::map<const SpeciesAtom *, std::pair<int, NETAMatchedGroup>> forwardMatches, backwardMatches;
            auto totalAttemptedNodeMatches = 0;

            // Loop over defined nodes
            for (const auto &node : nodes_)
            {
                // Cast up the ring node
                auto ringAtomNode = std::dynamic_pointer_cast<NETARingAtomNode>(node);
                auto nForwardNodeMatches = 0, nBackwardNodeMatches = 0;

                // Loop until we find the required number of matches, we fail to match, or we run out of ring atoms
                do
                {
                    ++totalAttemptedNodeMatches;

                    // Test forward ring iterator?
                    if (forwardIt != ring.atoms().end())
                    {
                        NETAMatchedGroup forwardMatchPath;
                        auto forwardScore = ringAtomNode->matches(*forwardIt, forwardMatchPath);
                        if (forwardScore == NETANode::NoMatch)
                            forwardIt = ring.atoms().end();
                        else
                        {
                            forwardMatches[*forwardIt] = {forwardScore, forwardMatchPath};
                            ++nForwardNodeMatches;
                        }
                    }

                    // Test backward ring iterator?
                    if (backwardIt != ring.atoms().end())
                    {
                        NETAMatchedGroup backwardMatchPath;
                        auto backwardScore = ringAtomNode->matches(*backwardIt, backwardMatchPath);
                        if (backwardScore == NETANode::NoMatch)
                            backwardIt = ring.atoms().end();
                        else
                        {
                            backwardMatches[*backwardIt] = {backwardScore, backwardMatchPath};
                            ++nBackwardNodeMatches;
                        }
                    }

                    // Iterate until we have gone all around the ring
                    if (forwardIt != ring.atoms().end())
                    {
                        forwardIt = ++forwardIt == ring.atoms().end() ? ring.atoms().begin() : forwardIt;
                        if (forwardIt == ringAtomIt)
                            forwardIt = ring.atoms().end();
                    }
                    if (backwardIt != ring.atoms().end())
                    {
                        backwardIt = backwardIt == ring.atoms().begin() ? ring.atoms().end() - 1 : --backwardIt;
                        if (backwardIt == ringAtomIt)
                            backwardIt = ring.atoms().end();
                    }

                    // Test to see if we have the requested number of repeats so can break out early
                    if (ringAtomNode->validRepeatCount(nForwardNodeMatches) ||
                        ringAtomNode->validRepeatCount(nBackwardNodeMatches))
                        break;
                } while (forwardIt != ring.atoms().end() || backwardIt != ring.atoms().end());

                // Confirm that the requested number of repeats has been met - we may have left the loop early due to not
                // finding a match
                if (!ringAtomNode->validRepeatCount(nForwardNodeMatches) &&
                    !ringAtomNode->validRepeatCount(nBackwardNodeMatches))
                {
                    forwardMatches.clear();
                    backwardMatches.clear();
                }

                // Check iterator validity
                if (forwardIt == ring.atoms().end() && backwardIt == ring.atoms().end())
                    break;
            }

            // Check iterator validity - if one persists here then we have a match to the ring
            if (totalAttemptedNodeMatches == forwardMatches.size() || totalAttemptedNodeMatches == backwardMatches.size())
            {
                // Get relevant atom matches
                const auto &matchedRingAtoms =
                    totalAttemptedNodeMatches == forwardMatches.size() ? forwardMatches : backwardMatches;

                // Store the ring information - total score and all matched atoms
                matchedRings[&ring] = {std::accumulate(matchedRingAtoms.begin(), matchedRingAtoms.end(), 0,
                                                       [](const auto &acc, const auto &jInfo)
                                                       { return acc + jInfo.second.first; }),
                                       {}};
                auto &totalRingMatch = matchedRings[&ring].second;
                for (auto &jInfo : matchedRingAtoms)
                {
                    auto &&[jScore, jMatchPath] = jInfo.second;
                    totalRingMatch.merge(jMatchPath);
                }

                // Matched the ring starting from this ring atom, so break out of the loop
                break;
            }
        }

        // Don't match more than we need to - check the repeatCount
        if (compareValues(matchedRings.size(), repeatCountOperator_, repeatCount_))
            break;
    }

    // Did we find the required number of ring matches?
    if (reverseLogic_ == compareValues(matchedRings.size(), repeatCountOperator_, repeatCount_))
        return NETANode::NoMatch;

    // Generate total score and add matched atoms to the path
    auto totalScore = 0;
    for (auto &ring : matchedRings)
    {
        auto &&[ringScore, ringMatchPath] = ring.second;

        totalScore += ringScore;

        // Track atoms matched in the neighbour branch
        matchPath.merge(ringMatchPath);
    }

    return totalScore;
}
