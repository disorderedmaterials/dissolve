// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
    virtual ConstNodeRef baseNode() = 0;
    // Set base node data
    virtual bool setData(ConstNodeRef node) = 0;
};

// Keyword managing ProcedureNode
template <class N> class NodeKeyword : public NodeKeywordBase
{
    public:
    NodeKeyword(std::shared_ptr<const N> &data, ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
        : NodeKeywordBase(parentNode, nodeType, onlyInScope), data_(data)
    {
    }
    NodeKeyword(std::shared_ptr<const N> &data, ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
        : NodeKeywordBase(parentNode, nodeClass, onlyInScope), data_(data)
    {
    }
    ~NodeKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::shared_ptr<const N> &data_;

    public:
    // Return reference to data
    std::shared_ptr<const N> &data() { return data_; }
    std::shared_ptr<const N> &data() const { return data_; }
    // Return base node
    ConstNodeRef baseNode() override { return data_; }
    // Set data
    bool setData(ConstNodeRef node) override
    {
        if (node == nullptr)
            data_ = nullptr;
        else
        {
            if (!validNode(node.get(), name()))
                return false;

            data_ = std::dynamic_pointer_cast<const N>(node);
            assert(data_);
        }

        return true;
    }

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Locate the named node
        auto node = findNode(parser.argsv(startArg));
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
    void removeReferencesTo(NodeRef node) override
    {
        if (data_ == node)
            data_ = nullptr;
    }
};
