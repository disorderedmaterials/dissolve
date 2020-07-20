// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "templates/reflist.h"
#include <memory>
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;
class NETAPresenceNode;
class NETAConnectionNode;
class NETARingNode;
class SpeciesAtom;

// NETA Node
class NETANode
{
    public:
    // Node types
    enum NodeType
    {
        BasicNode,
        ConnectionNode,
        LogicNode,
        PresenceNode,
        RingNode,
        RootNode,
        nNETANodeTypes
    };
    // Return enum options for Node Types
    static EnumOptions<NETANode::NodeType> nodeTypes();
    // Value Comparison Operators
    enum ComparisonOperator
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
    virtual ~NETANode();

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
    virtual bool addElementTarget(const Element &el);
    // Add forcefield type target to node
    virtual bool addFFTypeTarget(const ForcefieldAtomType &ffType);

    /*
     * Branching and Node Generation
     */
    protected:
    // Branch of nodes
    std::vector<std::shared_ptr<NETANode>> branch_;

    public:
    // Clear all nodes
    void clear();
    // Create connectivity node in the branch
    std::shared_ptr<NETAConnectionNode>
    createConnectionNode(std::vector<std::reference_wrapper<const Element>> targetElements = {},
                         std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes = {});
    // Create presence node in the branch
    std::shared_ptr<NETAPresenceNode>
    createPresenceNode(std::vector<std::reference_wrapper<const Element>> targetElements = {},
                       std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes = {});
    // Create ring node in the branch
    std::shared_ptr<NETARingNode> createRingNode();

    /*
     * Modifiers
     */
    public:
    // Return whether the specified modifier is valid for this node
    virtual bool isValidModifier(std::string_view s) const;
    // Set value and comparator for specified modifier
    virtual bool setModifier(std::string_view modifier, ComparisonOperator op, int value);

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

    public:
    // Set node to use reverse logic
    void setReverseLogic();
    // Evaluate the node and return its score
    virtual int score(const SpeciesAtom *i, RefList<const SpeciesAtom> &atomData) const;
};
