// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/base.h"
#include "keywords/nodeunderlay.h"
#include "procedure/nodes/node.h"
#include <tuple>

// Base class for NodeAndIntegerKeyword
class NodeAndIntegerKeywordBase : public NodeKeywordUnderlay, public KeywordBase
{
    public:
    NodeAndIntegerKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, nodeType, onlyInScope), KeywordBase(typeid(this))
    {
    }
    NodeAndIntegerKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, nodeClass, onlyInScope), KeywordBase(typeid(this))
    {
    }
    ~NodeAndIntegerKeywordBase() override = default;

    /*
     * Data
     */
    public:
    // Return base node data
    virtual const ProcedureNode *baseNode() = 0;
    // Set node
    virtual bool setNode(const ProcedureNode *node) = 0;
    // Set index
    virtual void setIndex(int i) = 0;
};

// Keyword with ProcedureNode and integer index
template <class N> class NodeAndIntegerKeyword : public NodeAndIntegerKeywordBase
{
    public:
    NodeAndIntegerKeyword(std::pair<const N *, int> &data, ProcedureNode *parentNode, ProcedureNode::NodeType nodeType,
                          bool onlyInScope)
        : NodeAndIntegerKeywordBase(parentNode, nodeType, onlyInScope), data_(data)
    {
    }
    NodeAndIntegerKeyword(std::pair<const N *, int> &data, ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass,
                          bool onlyInScope)
        : NodeAndIntegerKeywordBase(parentNode, nodeClass, onlyInScope), data_(data)
    {
    }
    ~NodeAndIntegerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::pair<const N *, int> &data_;

    public:
    // Return reference to vector of data
    std::pair<const N *, int> &data() { return data_; }
    const std::pair<const N *, int> &data() const { return data_; }
    // Return base node data
    const ProcedureNode *baseNode() override { return data_.first; }
    // Set node
    virtual bool setNode(const ProcedureNode *node)
    {
        if (node == nullptr)
            data_.first = nullptr;
        else
        {
            // Check the type
            if (!validNode(node, name()))
                return false;

            data_.first = dynamic_cast<const N *>(node);
            assert(data_.first);
        }
        setAsModified();
        return true;
    }
    // Set index
    virtual void setIndex(int i)
    {
        data_.second = i;
        setAsModified();
    }

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return 2; }
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Locate the named node in scope
        auto *node = findNode(parser.argsv(startArg));
        if (!node)
            return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        if (parser.hasArg(startArg + 1))
            setIndex(parser.argi(startArg + 1));

        return setNode(node);
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        // No need to write the keyword if the node pointer is null
        if (data_.first == nullptr)
            return true;

        return parser.writeLineF("{}{}  '{}'  {}\n", prefix, KeywordBase::name(), data_.first->name(), data_.second);
    }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied ProcedureNode in the contained data
    void removeReferencesTo(ProcedureNode *node) override
    {
        if (data_.first == node)
            data_.first = nullptr;
    }
};
