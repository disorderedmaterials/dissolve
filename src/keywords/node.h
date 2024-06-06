// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "keywords/base.h"
#include "keywords/nodeUnderlay.h"
#include "procedure/nodes/node.h"

// Base class for NodeKeyword
class NodeKeywordBase : public NodeKeywordUnderlay, public KeywordBase
{
    public:
    NodeKeywordBase(ProcedureNode *parentNode, const ProcedureNode::NodeTypeVector &allowedNodeTypes, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, allowedNodeTypes, onlyInScope), KeywordBase(typeid(this))
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
    // Validate current data, returning false if invalid data had to be pruned
    bool validate() override
    {
        if (validNode(baseNode().get(), name()))
            return true;

        setData(nullptr);

        return false;
    }
};

// Keyword managing ProcedureNode
template <class N> class NodeKeyword : public NodeKeywordBase
{
    public:
    NodeKeyword(std::shared_ptr<const N> &data, ProcedureNode *parentNode,
                const ProcedureNode::NodeTypeVector &allowedNodeTypes, bool onlyInScope)
        : NodeKeywordBase(parentNode, allowedNodeTypes, onlyInScope), data_(data)
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

    // Has not changed from initial value
    bool isDefault() const override { return data_ == nullptr; }

    // Express as a serialisable value
    SerialisedValue serialise() const override { return data_->name(); }

    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData)
    {
        auto child = findNode(std::string_view(std::string(node.as_string())));
        setData(child);
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
