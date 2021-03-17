// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/data.h"
#include "procedure/nodes/node.h"
#include "templates/array.h"

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword with Node Array
class NodeArrayKeywordBase
{
    public:
    NodeArrayKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope, bool uniqueNodes);
    NodeArrayKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, int fixedArraySize, bool onlyInScope,
                         bool uniqueNodes);
    virtual ~NodeArrayKeywordBase();

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    public:
    // Return parent ProcedureNode
    ProcedureNode *parentNode() const;

    /*
     * Target Nodes and Array Size
     */
    private:
    // Target node type to allow
    ProcedureNode::NodeType nodeType_;
    // Whether to accept nodes within scope only
    bool onlyInScope_;
    // Whether any number of nodes is allowed in the array
    bool isVariableSize_;
    // Number of nodes to enforce in a fixed array
    int fixedArraySize_;
    // Whether nodes in the array must all be unique
    bool nodesAreUnique_;

    public:
    // Return target node type to allow
    ProcedureNode::NodeType nodeType() const;
    // Return whether to accept nodes within scope only
    bool onlyInScope() const;
    // Return whether this is a variable size array
    bool isVariableSize() const;
    // Return size of fixed array (if relevant)
    int fixedArraySize() const;
    // Return whether nodes in the array must all be unique
    bool nodesAreUnique() const;
    // Add the specified node to the list
    virtual bool addNode(ProcedureNode *node) = 0;
    // Return the current array (as ProcedureNodes)
    virtual Array<ProcedureNode *> procedureNodes() const = 0;
    // Return if the specified node is in the current list
    virtual bool hasNode(ProcedureNode *node) = 0;
};

// Keyword with ProcedureNode RefList
template <class N> class NodeArrayKeyword : public NodeArrayKeywordBase, public KeywordData<Array<N *> &>
{
    public:
    NodeArrayKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope, bool uniqueNodes,
                     Array<N *> &nodeArray)
        : NodeArrayKeywordBase(parentNode, nodeType, onlyInScope, uniqueNodes), KeywordData<Array<N *> &>(
                                                                                    KeywordBase::NodeArrayData, nodeArray)
    {
        // Initialise the array
        nodeArray.clear();
    }
    NodeArrayKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, int fixedArraySize, bool onlyInScope,
                     bool uniqueNodes, Array<N *> &nodeArray)
        : NodeArrayKeywordBase(parentNode, nodeType, fixedArraySize, onlyInScope, uniqueNodes), KeywordData<Array<N *> &>(
                                                                                                    KeywordBase::NodeArrayData,
                                                                                                    nodeArray)
    {
        // Initialise the array if it looks like it needs it
        if (nodeArray.nItems() != fixedArraySize)
        {
            nodeArray.initialise(fixedArraySize);
            for (auto n = 0; n < fixedArraySize; ++n)
                nodeArray[n] = nullptr;
        }
    }
    ~NodeArrayKeyword() {}

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override
    {
        return NodeArrayKeywordBase::isVariableSize() ? 1 : NodeArrayKeywordBase::fixedArraySize();
    }
    // Return maximum number of arguments accepted
    int maxArguments() const override
    {
        return NodeArrayKeywordBase::isVariableSize() ? 99 : NodeArrayKeywordBase::fixedArraySize();
    }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData)
    {
        if (!parentNode())
            return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n",
                                    KeywordBase::name());

        // Loop over arguments
        for (auto n = startArg; n < parser.nArgs(); ++n)
        {
            // Locate the named node - don't prune by type yet (we'll check that in setNode())
            ProcedureNode *node = onlyInScope() ? parentNode()->nodeInScope(parser.argsv(startArg))
                                                : parentNode()->nodeExists(parser.argsv(startArg));
            if (!node)
                return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                        KeywordBase::name());

            if (!addNode(node))
                return false;
        }

        return true;
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        if (KeywordData<Array<N *> &>::data_.nItems() == 0)
            return true;

        std::string nodes;
        for (auto n = 0; n < KeywordData<Array<N *> &>::data_.nItems(); ++n)
        {
            N *node = KeywordData<Array<N *> &>::data_[n];
            nodes += fmt::format("  '{}'", node ? node->name() : "???");
        }

        if (!parser.writeLineF("{}{}  {}\n", prefix, KeywordBase::name(), nodes))
            return false;

        return true;
    }

    /*
     * Target Nodes Array
     */
    public:
    // Add the specified node to the array (only possible if we are a variable size array)
    bool addNode(ProcedureNode *node)
    {
        if (!node)
            return false;

        // Check if array extension is allowed
        if (!NodeArrayKeywordBase::isVariableSize())
            return Messenger::error("Can't dynamically add a node to the fixed-size NodeArray keyword '{}'.\n",
                                    KeywordBase::name());

        if (node->type() != nodeType())
            return Messenger::error("Node '{}' is of type {}, but the {} keyword requires a node of type {}.\n", node->name(),
                                    ProcedureNode::nodeTypes().keyword(node->type()), KeywordBase::name(),
                                    ProcedureNode::nodeTypes().keyword(nodeType()));

        // Cast up the node
        N *castNode = dynamic_cast<N *>(node);
        if (!castNode)
            return false;

        // If we only accept unique nodes over all array elements, check that this node is not already in the array
        if (NodeArrayKeywordBase::nodesAreUnique() && (indexOfNode(node) != -1))
            return Messenger::error("Node '{}' is already present in the array for keyword {}.\n", castNode->name(),
                                    KeywordBase::name());

        // Add the node to the list
        KeywordData<Array<N *> &>::data_.add(castNode);

        KeywordData<Array<N *> &>::set_ = true;

        return true;
    }
    // Set the provided node in the specified array position
    bool setNode(ProcedureNode *node, int arrayIndex)
    {
        if (!node)
            return false;

        // Check array index provided
        if ((arrayIndex < 0) || (arrayIndex >= KeywordData<Array<N *> &>::data_.nItems()))
            return Messenger::error("Array index {} is out of bounds for the NodeArray keyword '{}'.\n", arrayIndex,
                                    KeywordBase::name());

        if (node->type() != nodeType())
            return Messenger::error("Node '{}' is of type {}, but the {} keyword requires a node of type {}.\n", node->name(),
                                    ProcedureNode::nodeTypes().keyword(node->type()), KeywordBase::name(),
                                    ProcedureNode::nodeTypes().keyword(nodeType()));

        // Cast up the node
        N *castNode = dynamic_cast<N *>(node);
        if (!castNode)
            return false;

        // If we only accept unique nodes over all array elements, check that this node is not already in the array
        if (NodeArrayKeywordBase::nodesAreUnique() && (indexOfNode(node) != -1))
            return Messenger::error("Node '{}' is already present in the array for keyword {}.\n", castNode->name(),
                                    KeywordBase::name());

        // Set the node pointer in the specified position
        KeywordData<Array<N *> &>::data_[arrayIndex] = castNode;

        KeywordData<Array<N *> &>::set_ = true;

        return true;
    }
    // Set the specified array position to NULL
    bool clearNode(int arrayIndex)
    {
        // Check array index provided
        if ((arrayIndex < 0) || (arrayIndex >= KeywordData<Array<N *> &>::data_.nItems()))
            return Messenger::error(
                "Array index {} is out of bounds for the NodeArray keyword '{}', so can't clear the element.\n", arrayIndex,
                KeywordBase::name());

        // Set the node pointer in the specified position
        KeywordData<Array<N *> &>::data_[arrayIndex] = nullptr;

        KeywordData<Array<N *> &>::set_ = true;

        return true;
    }
    // Return index of the specified node, if it is in the array
    int indexOfNode(ProcedureNode *node) const
    {
        for (auto n = 0; n < KeywordData<Array<N *> &>::data_.nItems(); ++n)
            if (KeywordData<Array<N *> &>::data_.at(n) == node)
                return n;

        return -1;
    }
    // Return the current array (as ProcedureNodes)
    Array<ProcedureNode *> procedureNodes() const
    {
        Array<ProcedureNode *> nodes(KeywordData<Array<N *> &>::data_.nItems());

        for (auto n = 0; n < KeywordData<Array<N *> &>::data_.nItems(); ++n)
            nodes[n] = KeywordData<Array<N *> &>::data_.at(n);

        return nodes;
    }
    // Return the current array
    Array<N *> nodes() const { return KeywordData<Array<N *> &>::data_; }
    // Return if the specified node is in the current list
    bool hasNode(ProcedureNode *node) { return (indexOfNode(node) != -1); }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied ProcedureNode in the contained data
    void removeReferencesTo(ProcedureNode *node)
    {
        // Check the node type
        if (node->type() != nodeType())
            return;

        // Cast up the node
        N *castNode = dynamic_cast<N *>(node);
        if (!castNode)
            return;

        // Loop over array items
        for (auto n = 0; n < KeywordData<Array<N *> &>::data_.nItems(); ++n)
            if (KeywordData<Array<N *> &>::data_[n] == castNode)
                KeywordData<Array<N *> &>::data_[n] = nullptr;
    }
};
