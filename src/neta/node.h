// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "data/elements.h"
#include <memory>
#include <vector>

// Forward Declarations
class ForcefieldAtomType;
class NETACharacterNode;
class NETAConnectionNode;
class NETADefinition;
class NETAOrNode;
class NETAPresenceNode;
class NETARingNode;
class SpeciesAtom;

// NETA Node
class NETANode
{
    public:
    // Node types
    enum class NodeType
    {
        Basic,
        Character,
        Connection,
        Or,
        Presence,
        Ring,
        Root
    };
    // Return enum options for Node Types
    static EnumOptions<NETANode::NodeType> nodeTypes();
    // Value Comparison Operators
    enum class ComparisonOperator
    {
        EqualTo,
        NotEqualTo,
        GreaterThan,
        LessThan,
        GreaterThanEqualTo,
        LessThanEqualTo
    };
    // Return enum options for Comparison Operators
    static EnumOptions<NETANode::ComparisonOperator> comparisonOperators();
    // Special Scoring Results
    enum NETAResult
    {
        NoMatch = -1
    };
    NETANode(NETADefinition *parent, NodeType type);
    NETANode() = default;
    virtual ~NETANode() = default;

    /*
     * Node Type and Parent
     */
    protected:
    // Type of node
    NodeType nodeType_;
    // Pointer to parent definition
    NETADefinition *parent_;

    public:
    // Return node type
    NodeType nodeType() const;
    // Return parent definition
    NETADefinition *parent() const;

    /*
     * Atom Targets
     */
    public:
    // Add element target to node
    virtual bool addElementTarget(Elements::Element Z);
    // Add forcefield type target to node
    virtual bool addFFTypeTarget(const ForcefieldAtomType &ffType);

    /*
     * Node Sequence
     */
    public:
    // Sequence Definition
    using NETASequence = std::vector<std::shared_ptr<NETANode>>;

    protected:
    // Node sequence
    NETASequence nodes_;

    public:
    // Set node sequence
    void setNodes(NETASequence nodes);

    /*
     * Modifiers
     */
    public:
    // Return whether the specified modifier is valid for this node
    virtual bool isValidModifier(std::string_view s) const;
    // Set value and comparator for specified modifier
    virtual bool setModifier(std::string_view modifier, ComparisonOperator op, int value);

    /*
     * Options
     */
    public:
    // Return whether the specified option is valid for this node
    virtual bool isValidOption(std::string_view s) const;
    // Set value and comparator for specified modifier
    virtual bool setOption(std::string_view option, ComparisonOperator op, std::string_view value);

    /*
     * Flags
     */
    public:
    // Return whether the specified flag is valid for this node
    virtual bool isValidFlag(std::string_view s) const;
    // Set specified flag
    virtual bool setFlag(std::string_view flag, bool state);

    /*
     * Value Comparison
     */
    public:
    // Return result of comparison between values provided
    static bool compareValues(int lhsValue, ComparisonOperator op, int rhsValue);

    /*
     * Scoring
     */
    protected:
    // Whether to use reverse logic when returning the final value
    bool reverseLogic_;

    protected:
    // Evaluate the provided sequence and return a score
    static int sequenceScore(const NETANode::NETASequence &sequence, const SpeciesAtom *i,
                             std::vector<const SpeciesAtom *> &atomData);

    public:
    // Set node to use reverse logic
    void setReverseLogic();
    // Evaluate the node and return its score
    virtual int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &atomData) const;
};
