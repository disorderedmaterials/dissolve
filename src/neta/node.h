// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "templates/list.h"
#include "templates/listitem.h"
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
class NETANode : public ListItem<NETANode>
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
     * Branching and Node Generation
     */
    protected:
    // Branch of nodes
    List<NETANode> branch_;

    public:
    // Clear all nodes
    void clear();
    // Return last node of branch
    NETANode *lastBranchNode();
    // Return number of nodes defined in branch
    int nBranchNodes() const;
    // Create connectivity node in the branch
    NETAConnectionNode *createConnectionNode(std::vector<Element *> targetElements,
                                             std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes);
    // Create presence node in the branch
    NETAPresenceNode *createPresenceNode(std::vector<Element *> targetElements,
                                         std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes);
    // Create ring node in the branch
    NETARingNode *createRingNode();

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
