// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "keywords/base.h"
#include "keywords/nodeUnderlay.h"
#include "procedure/nodes/node.h"
#include "templates/algorithms.h"

// Base class for NodeVectorKeyword
class NodeVectorKeywordBase : public NodeKeywordUnderlay, public KeywordBase
{
    public:
    NodeVectorKeywordBase(ProcedureNode *parentNode, const ProcedureNode::NodeTypeVector &allowedNodeTypes, bool onlyInScope)
        : NodeKeywordUnderlay(parentNode, allowedNodeTypes, onlyInScope), KeywordBase(typeid(this))
    {
    }
    ~NodeVectorKeywordBase() override = default;

    /*
     * Data
     */
    public:
    // Add node to vector
    virtual bool addNode(ConstNodeRef node) = 0;
    // Remove node from vector
    virtual bool removeNode(ConstNodeRef node) = 0;
    // Return whether specified node is currently in the vector
    virtual bool isPresent(ConstNodeRef node) const = 0;
    // Return plain ProcedureNode vector
    virtual std::vector<ConstNodeRef> nodes() const = 0;
    // Validate current data, returning false if invalid data had to be pruned
    bool validate() override
    {
        auto oldData = nodes();
        for (auto &oldNode : oldData)
            if (!validNode(oldNode.get(), name()))
                removeNode(oldNode);

        return oldData.size() == nodes().size();
    }
    SerialisedValue serialise() const override { throw std::runtime_error("Cannot serialise NodeVectorKeywordBase"); }
};

// Keyword managing vector of ProcedureNode
template <class N> class NodeVectorKeyword : public NodeVectorKeywordBase
{
    public:
    NodeVectorKeyword(ConstNodeVector<N> &data, ProcedureNode *parentNode,
                      const ProcedureNode::NodeTypeVector &allowedNodeTypes, bool onlyInScope)
        : NodeVectorKeywordBase(parentNode, allowedNodeTypes, onlyInScope), data_(data)
    {
    }
    ~NodeVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to vector of data
    ConstNodeVector<N> &data_;

    public:
    // Return reference to vector of data
    ConstNodeVector<N> &data() { return data_; }
    const ConstNodeVector<N> &data() const { return data_; }
    // Set vector data
    bool setData(const ConstNodeVector<N> &v)
    {
        for (auto &node : v)
            if (!addNode(node))
                return false;
        return true;
    }
    // Add node to vector
    bool addNode(ConstNodeRef node) override
    {
        auto castNode = std::dynamic_pointer_cast<const N>(node);
        assert(castNode);
        if (std::find(data_.begin(), data_.end(), castNode) != data_.end())
            return false;

        if (!validNode(castNode.get(), name()))
            return false;

        data_.emplace_back(castNode);

        return true;
    }
    // Remove node from vector
    bool removeNode(ConstNodeRef node) override
    {
        auto castNode = std::dynamic_pointer_cast<const N>(node);
        assert(castNode);
        auto it = std::find(data_.begin(), data_.end(), castNode);
        if (it == data_.end())
            return false;
        data_.erase(it);

        return true;
    }
    // Return whether specified node is currently in the vector
    bool isPresent(ConstNodeRef node) const override { return std::find(data_.begin(), data_.end(), node) != data_.end(); }
    // Return plain ProcedureNode vector
    std::vector<ConstNodeRef> nodes() const override
    {
        std::vector<ConstNodeRef> result;
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
            ConstNodeRef node = findNode(parser.argsv(n));
            if (!node)
                return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(n), name());

            if (!validNode(node.get(), name()))
                return false;

            data_.push_back(std::dynamic_pointer_cast<const N>(node));
        }

        return true;
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        if (data_.empty())
            return true;

        if (!parser.writeLineF("{}{}  {}\n", prefix, name(),
                               joinStrings(data_, " ", [](const auto node) { return fmt::format("'{}'", node->name()); })))
            return false;

        return true;
    }

    // Express as a serialisable value
    SerialisedValue serialise() const override
    {
        return fromVector(data_, [](const auto &item) { return item->name(); });
    }

    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override
    {
        toVector(node,
                 [this](const auto &item)
                 {
                     // Locate the named node - don't prune by type yet (we'll check that in setNode())
                     // ConstNodeRef noderef = findNode(std::string(n.as_string()));
                     std::string nodeName = item.as_string();

                     ConstNodeRef noderef = findNode(nodeName);
                     if (!noderef)
                         throw toml::type_error(fmt::format("Node '{}' given to keyword {} doesn't exist.\n",
                                                            std::string(item.as_string()), name()),
                                                item.location());

                     if (!validNode(noderef.get(), name()))
                         throw toml::type_error(fmt::format("Invalid node: {}", name()), item.location());

                     data_.push_back(std::dynamic_pointer_cast<const N>(noderef));
                 });
    }

    // Has not changed from initial value
    bool isDefault() const override { return data_.empty(); }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied ProcedureNode in the contained data
    void removeReferencesTo(NodeRef node) override { data_.erase(std::remove(data_.begin(), data_.end(), node), data_.end()); }
};
