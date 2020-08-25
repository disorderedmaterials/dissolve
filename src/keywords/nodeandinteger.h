/*
    *** Keyword - Node and Integer
    *** src/keywords/nodeandindex.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/lineparser.h"
#include "keywords/data.h"
#include "procedure/nodes/node.h"
#include <tuple>

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword with ProcedureNode and integer index base class
class NodeAndIntegerKeywordBase
{
    public:
    NodeAndIntegerKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope);
    virtual ~NodeAndIntegerKeywordBase();

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
     * Target Node
     */
    private:
    // Target node type to allow
    ProcedureNode::NodeType nodeType_;
    // Whether to accept nodes within scope only
    bool onlyInScope_;

    public:
    // Return target node type to allow
    ProcedureNode::NodeType nodeType() const;
    // Return whether to accept nodes within scope only
    bool onlyInScope() const;
    // Set the target node
    virtual bool setNode(ProcedureNode *node) = 0;
    // Return the current target node
    virtual ProcedureNode *node() const = 0;

    /*
     * Associated Index
     */
    public:
    // Set target index
    virtual void setIndex(int index) = 0;
    // Return target index
    virtual int index() const = 0;

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    virtual int optionMask() const = 0;
};

// Keyword with ProcedureNode and integer index
template <class N> class NodeAndIntegerKeyword : public NodeAndIntegerKeywordBase, public KeywordData<std::tuple<N *, int>>
{
    public:
    NodeAndIntegerKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope, N *node)
        : NodeAndIntegerKeywordBase(parentNode, nodeType, onlyInScope), KeywordData<std::tuple<N *, int>>(
                                                                            KeywordBase::NodeAndIntegerData,
                                                                            std::tuple<N *, int>(node, -1))
    {
    }
    NodeAndIntegerKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope, N *node, int index)
        : NodeAndIntegerKeywordBase(parentNode, nodeType, onlyInScope), KeywordData<std::tuple<N *, int>>(
                                                                            KeywordBase::NodeAndIntegerData,
                                                                            std::tuple<N *, int>(node, index))
    {
    }
    ~NodeAndIntegerKeyword() {}

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const { return 1; }
    // Return maximum number of arguments accepted
    int maxArguments() const { return 1; }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData)
    {
        if (!parentNode())
            return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n",
                                    KeywordBase::name());

        // Locate the named node in scope - don't prune by type yet (we'll check that in setNode())
        ProcedureNode *node = onlyInScope() ? parentNode()->nodeInScope(parser.argsv(startArg))
                                            : parentNode()->nodeExists(parser.argsv(startArg));
        if (!node)
            return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        return setNode(node);
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
    {
        // Grab the node pointer
        const N *node = std::get<0>(KeywordData<std::tuple<N *, int>>::data_);

        // If an index was set, write it after the node name
        if (std::get<1>(KeywordData<std::tuple<N *, int>>::data_) >= 0)
        {
            if (!parser.writeLineF("{}{}  '{}'\n", prefix, KeywordBase::name(), node ? node->name() : "???"))
                return false;
        }
        else
        {
            if (!parser.writeLineF("{}{}  '{}'  {}\n", prefix, KeywordBase::name(), node ? node->name() : "???",
                                   std::get<1>(KeywordData<std::tuple<N *, int>>::data_)))
                return false;
        }

        return true;
    }

    /*
     * Target Node
     */
    public:
    // Set the target node
    bool setNode(ProcedureNode *node)
    {
        if (!node)
            return false;

        if (!node->isType(nodeType()))
            return Messenger::error("Node '{}' is of type {}, but the {} keyword requires a node of type {}.\n", node->name(),
                                    ProcedureNode::nodeTypes().keyword(node->type()), KeywordBase::name(),
                                    ProcedureNode::nodeTypes().keyword(nodeType()));

        std::get<0>(KeywordData<std::tuple<N *, int>>::data_) = dynamic_cast<N *>(node);

        KeywordData<std::tuple<N *, int>>::set_ = true;

        return true;
    }
    // Return the current target node
    ProcedureNode *node() const { return std::get<0>(KeywordData<std::tuple<N *, int>>::data_); }

    /*
     * Associated Index
     */
    public:
    // Set target index
    void setIndex(int index)
    {
        std::get<1>(KeywordData<std::tuple<N *, int>>::data_) = index;

        KeywordData<std::tuple<N *, int>>::set_ = true;
    }

    // Return target index
    int index() const { return std::get<1>(KeywordData<std::tuple<N *, int>>::data_); }

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    int optionMask() const { return KeywordBase::optionMask(); }

    /*
     * Object Management
     */
    protected:
};
