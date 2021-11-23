// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/base.h"
#include "keywords/nodeunderlay.h"
#include "procedure/nodes/node.h"
#include "templates/algorithms.h"

// Base class for NodeVectorKeyword
class NodeVectorKeywordBase : public NodeKeywordUnderlay, public KeywordBase
{
    public:
    NodeVectorKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, nodeType, onlyInScope), KeywordBase(typeid(this))
    {
    }
    NodeVectorKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, nodeClass, onlyInScope), KeywordBase(typeid(this))
    {
    }
    ~NodeVectorKeywordBase() override = default;

    /*
     * Data
     */
    public:
    // Add node to vector
    virtual bool addNode(const ProcedureNode *node) = 0;
    // Remove node from vector
    virtual bool removeNode(const ProcedureNode *node) = 0;
    // Return whether specified node is currently in the vector
    virtual bool isPresent(const ProcedureNode *node) const = 0;
    // Return plain ProcedureNode vector
    virtual std::vector<const ProcedureNode *> nodes() const = 0;
};

// Keyword managing vector of ProcedureNode
template <class N> class NodeVectorKeyword : public NodeVectorKeywordBase
{
    public:
    NodeVectorKeyword(std::vector<const N *> &data, ProcedureNode *parentNode, ProcedureNode::NodeType nodeType,
                      bool onlyInScope)
        : NodeVectorKeywordBase(parentNode, nodeType, onlyInScope), data_(data)
    {
    }
    NodeVectorKeyword(std::vector<const N *> &data, ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass,
                      bool onlyInScope)
        : NodeVectorKeywordBase(parentNode, nodeType, onlyInScope), data_(data)
    {
    }
    ~NodeVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to vector of data
    std::vector<const N *> &data_;

    public:
    // Return reference to vector of data
    std::vector<const N *> &data() { return data_; }
    const std::vector<const N *> &data() const { return data_; }
    // Add node to vector
    bool addNode(const ProcedureNode *node) override
    {
        const auto *castNode = dynamic_cast<const N *>(node);
        assert(castNode);
        if (std::find(data_.begin(), data_.end(), castNode) != data_.end())
            return false;
        data_.emplace_back(castNode);
        setAsModified();
        return true;
    }
    // Remove node from vector
    bool removeNode(const ProcedureNode *node) override
    {
        const auto *castNode = dynamic_cast<const N *>(node);
        assert(castNode);
        auto it = std::find(data_.begin(), data_.end(), castNode);
        if (it == data_.end())
            return false;
        data_.erase(it);
        setAsModified();
        return true;
    }
    // Return whether specified node is currently in the vector
    bool isPresent(const ProcedureNode *node) const override
    {
        return std::find(data_.begin(), data_.end(), node) != data_.end();
    }
    // Return plain ProcedureNode vector
    std::vector<const ProcedureNode *> nodes() const
    {
        std::vector<const ProcedureNode *> result;
        std::copy(data_.begin(), data_.end(), std::back_inserter(result));
        return result;
    }

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return std::nullopt; }
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Loop over arguments
        for (auto n = startArg; n < parser.nArgs(); ++n)
        {
            // Locate the named node - don't prune by type yet (we'll check that in setNode())
            auto *node = findNode(parser.argsv(n));
            if (!node)
                return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(n), name());

            if (!validNode(node, name()))
                return false;

            data_.push_back(dynamic_cast<const N *>(node));
        }

        return true;
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        if (data_.empty())
            return true;

        if (!parser.writeLineF("{}{}  {}\n", prefix, name(),
                               joinStrings(data_, " ", [](const auto *node) { return fmt::format("'{}'", node->name()); })))
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
        // Check the node type
        if (node->type() != nodeType())
            return;

        data_.erase(std::remove(data_.begin(), data_.end(), node), data_.end());
    }
};
