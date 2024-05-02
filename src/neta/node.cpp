// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/node.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "neta/character.h"
#include "neta/connection.h"
#include "neta/or.h"
#include "neta/ring.h"
#include "neta/ringAtom.h"

// Return enum options for NodeTypes
EnumOptions<NETANode::NodeType> NETANode::nodeTypes()
{
    return EnumOptions<NETANode::NodeType>("NodeTypes", {{NodeType::Base, "Base"},
                                                         {NodeType::BondCount, "BondCount"},
                                                         {NodeType::Character, "Character"},
                                                         {NodeType::Connection, "Connection"},
                                                         {NodeType::Geometry, "Geometry"},
                                                         {NodeType::HydrogenCount, "HydrogenCount"},
                                                         {NodeType::Or, "Or"},
                                                         {NodeType::Ring, "Ring"},
                                                         {NodeType::RingAtom, "RingAtom"},
                                                         {NodeType::Root, "Root"}});
}

// Return enum options for ComparisonOperator
EnumOptions<NETANode::ComparisonOperator> NETANode::comparisonOperators()
{
    return EnumOptions<NETANode::ComparisonOperator>("ComparisonOperator", {{ComparisonOperator::EqualTo, "="},
                                                                            {ComparisonOperator::NotEqualTo, "!="},
                                                                            {ComparisonOperator::GreaterThan, ">"},
                                                                            {ComparisonOperator::LessThan, "<"},
                                                                            {ComparisonOperator::GreaterThanEqualTo, ">="},
                                                                            {ComparisonOperator::LessThanEqualTo, "<="}});
}

NETANode::NETANode(NETADefinition *parent, NETANode::NodeType type)
{
    reverseLogic_ = false;
    parent_ = parent;
    nodeType_ = type;
}

/*
 * Node Type and Parent
 */

// Return node type
NETANode::NodeType NETANode::nodeType() const { return nodeType_; }

// Return parent
NETADefinition *NETANode::parent() const { return parent_; }

/*
 * Atom Targets
 */

// Add element target to node
bool NETANode::addElementTarget(Elements::Element Z)
{
    return Messenger::error("NETA {} node does not accept element targets.\n", nodeTypes().keyword(nodeType_));
}

// Add forcefield type target to node
bool NETANode::addFFTypeTarget(const ForcefieldAtomType &ffType)
{
    return Messenger::error("NETA {} node does not accept forcefield atomtype targets.\n", nodeTypes().keyword(nodeType_));
}

/*
 * Node Sequence
 */

// Set node sequence
void NETANode::setNodes(NETASequence nodes) { nodes_ = std::move(nodes); }

/*
 * Modifiers
 */

// Return whether the specified modifier is valid for this node
bool NETANode::isValidModifier(std::string_view s) const { return false; }

// Set value and comparator for specified modifier
bool NETANode::setModifier(std::string_view modifier, ComparisonOperator op, int value) { return false; }

/*
 * Options
 */

// Return whether the specified option is valid for this node
bool NETANode::isValidOption(std::string_view s) const { return false; }

// Set value and comparator for specified modifier
bool NETANode::setOption(std::string_view option, ComparisonOperator op, std::string_view value) { return false; }

/*
 * Flags
 */

// Return whether the specified flag is valid for this node
bool NETANode::isValidFlag(std::string_view s) const { return false; }

// Set specified flag
bool NETANode::setFlag(std::string_view flag, bool state) { return false; }

/*
 * Identifier Names
 */

// Add identifier to this node, returning if it is already in use
bool NETANode::addIdentifier(std::string_view s)
{
    auto it = std::find(identifiers_.begin(), identifiers_.end(), s);
    if (it != identifiers_.end())
        return false;

    identifiers_.emplace_back(s);

    return true;
}

// Return identifying names associated to this node
const std::vector<std::string> &NETANode::identifiers() const { return identifiers_; }

/*
 * Value Comparison
 */

// Return result of comparison between values provided
bool NETANode::compareValues(int lhsValue, ComparisonOperator op, int rhsValue)
{
    auto result = false;

    switch (op)
    {
        case (ComparisonOperator::EqualTo):
            result = (lhsValue == rhsValue);
            break;
        case (ComparisonOperator::NotEqualTo):
            result = (lhsValue != rhsValue);
            break;
        case (ComparisonOperator::GreaterThan):
            result = (lhsValue > rhsValue);
            break;
        case (ComparisonOperator::LessThan):
            result = (lhsValue < rhsValue);
            break;
        case (ComparisonOperator::GreaterThanEqualTo):
            result = (lhsValue >= rhsValue);
            break;
        case (ComparisonOperator::LessThanEqualTo):
            result = (lhsValue <= rhsValue);
            break;
        default:
            Messenger::error("Unrecognised operator (id = {}) in NETANode::valueComparison.\n", (int)op);
            break;
    }

    return result;
}

/*
 * Scoring
 */

// Evaluate the provided sequence and return a score
int NETANode::sequenceScore(const NETANode::NETASequence &sequence, const SpeciesAtom *i, NETAMatchedGroup &matchPath)
{
    auto totalScore = 0;
    auto newMatchPath = matchPath;
    newMatchPath.insert(i);

    // Loop over nodes in sequence
    for (const auto &node : sequence)
    {
        // Get the score from the node, returning early if NoMatch is encountered
        auto nodeScore = node->score(i, newMatchPath);
        if (nodeScore == NETANode::NoMatch)
            return NETANode::NoMatch;

        totalScore += nodeScore;
    }

    // Set the new match path
    matchPath = newMatchPath;

    return totalScore;
}

// Set node to use reverse logic
void NETANode::setReverseLogic() { reverseLogic_ = true; }

// Evaluate the node and return its score
int NETANode::score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const
{
    auto branchScore = sequenceScore(nodes_, i, matchPath);

    // Add local identifiers to the match data
    if (branchScore != NETANode::NoMatch && !reverseLogic_)
        for (auto &id : identifiers())
            matchPath.addIdentifier(i, id);

    return reverseLogic_ ? (branchScore == NETANode::NoMatch ? 1 : NETANode::NoMatch) : branchScore;
}
