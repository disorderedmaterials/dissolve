// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/base.h"
#include "keywords/nodeunderlay.h"
#include "procedure/nodes/node.h"

// Base class for NodeKeyword
class NodeKeywordBase : public NodeKeywordUnderlay, public KeywordBase
{
    public:
    NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, nodeType, onlyInScope), KeywordBase(typeid(this))
    {
    }
    NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, nodeClass, onlyInScope), KeywordBase(typeid(this))
    {
    }
    ~NodeKeywordBase() override = default;

    /*
     * Data
     */
    public:
    // Return base node data
    virtual const ProcedureNode *baseNode() = 0;
    // Set base node data
    virtual bool setData(const ProcedureNode *node) = 0;
};

// Keyword with ProcedureNode
template <class N> class NodeKeyword : public NodeKeywordBase
{
    public:
    NodeKeyword(const N *&data, ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
        : NodeKeywordBase(parentNode, nodeType, onlyInScope), data_(data)
    {
    }
    NodeKeyword(const N *&data, ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
        : NodeKeywordBase(parentNode, nodeClass, onlyInScope), data_(data)
    {
    }
    ~NodeKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    const N *&data_;

    public:
    // Return reference to data
    const N *&data() { return data_; }
    const N *&data() const { return data_; }
    // Return base node
    const ProcedureNode *baseNode() { return data_; }
    // Set data
    bool setData(const ProcedureNode *node) override
    {
        if (node == nullptr)
            data_ = nullptr;
        else
        {
            if (!validNode(node, name()))
                return false;

            data_ = dynamic_cast<const N *>(node);
            assert(data_);
        }
        setAsModified();

        return true;
    }

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override { return 1; }
    // Return maximum number of arguments accepted
    int maxArguments() const override { return 1; }
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Locate the named node
        auto *node = findNode(parser.argsv(startArg));
        if (!node)
            return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        return setData(node);
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        // No need to write the keyword if the node pointer is null
        if (data_ == nullptr)
            return true;

        if (!parser.writeLineF("{}{}  '{}'\n", prefix, KeywordBase::name(), data_->name()))
            return false;

        return true;
    }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied ProcedureNode in the contained data
    void removeReferencesTo(ProcedureNode *node) override
    {
        if (data_ == node)
            data_ = nullptr;
    }
};
