// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "keywords/base.h"
#include "keywords/nodeUnderlay.h"
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
    virtual ConstNodeRef baseNode() const = 0;
    // Set node
    virtual bool setNode(ConstNodeRef node) = 0;
    // Set integer
    virtual void setInteger(int i) = 0;
    // Return integer
    virtual int integer() const = 0;
    // Validate current data, returning false if invalid data had to be pruned
    bool validate() override
    {
        if (validNode(baseNode().get(), name()))
            return true;

        setNode(nullptr);

        return false;
    }
    // Express as a serialisable value
    SerialisedValue serialise() const override { return {{"node", baseNode()->name()}, {"int", integer()}}; }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override
    {
        // Locate the named node in scope
        auto nodeName = toml::find<std::string>(node, "node");
        auto realNode = findNode(nodeName);
        if (!realNode)
            throw toml::type_error(fmt::format("Node '{}' given to keyword {} doesn't exist.\n", nodeName, KeywordBase::name()),
                                   node.location());

        setInteger(toml::find<int>(node, "int"));
        setNode(realNode);
    };
};

// Keyword managing ProcedureNode and integer index
template <class N> class NodeAndIntegerKeyword : public NodeAndIntegerKeywordBase
{
    public:
    NodeAndIntegerKeyword(std::pair<std::shared_ptr<const N>, int> &data, ProcedureNode *parentNode,
                          ProcedureNode::NodeType nodeType, bool onlyInScope)
        : NodeAndIntegerKeywordBase(parentNode, nodeType, onlyInScope), data_(data)
    {
    }
    NodeAndIntegerKeyword(std::pair<std::shared_ptr<const N>, int> &data, ProcedureNode *parentNode,
                          ProcedureNode::NodeClass nodeClass, bool onlyInScope)
        : NodeAndIntegerKeywordBase(parentNode, nodeClass, onlyInScope), data_(data)
    {
    }
    ~NodeAndIntegerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::pair<std::shared_ptr<const N>, int> &data_;

    public:
    // Return reference to vector of data
    std::pair<std::shared_ptr<const N>, int> &data() { return data_; }
    const std::pair<std::shared_ptr<const N>, int> &data() const { return data_; }
    // Return base node data
    std::shared_ptr<const ProcedureNode> baseNode() const override { return data_.first; }
    // Set node
    bool setNode(ConstNodeRef node) override
    {
        if (node == nullptr)
            data_.first = nullptr;
        else
        {
            // Check the type
            if (!validNode(node.get(), name()))
                return false;

            data_.first = std::dynamic_pointer_cast<const N>(node);
            assert(data_.first);
        }

        return true;
    }
    // Set integer
    void setInteger(int i) override { data_.second = i; }
    // Return integer
    int integer() const override { return data_.second; }

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
        auto node = findNode(parser.argsv(startArg));
        if (!node)
            return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        if (parser.hasArg(startArg + 1))
            setInteger(parser.argi(startArg + 1));

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
    void removeReferencesTo(NodeRef node) override
    {
        if (data_.first == node)
            data_.first = nullptr;
    }
};
