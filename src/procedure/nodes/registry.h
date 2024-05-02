// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include <map>

// Forward Declarations
class KeywordWidgetBase;
class QWidget;

// ProcedureNode Registry
class ProcedureNodeRegistry
{
    private:
    ProcedureNodeRegistry();

    public:
    ProcedureNodeRegistry(const ProcedureNodeRegistry &) = delete;
    ProcedureNodeRegistry(ProcedureNodeRegistry &&) = delete;
    ProcedureNodeRegistry &operator=(const ProcedureNodeRegistry &) = delete;
    ProcedureNodeRegistry &operator=(ProcedureNodeRegistry &&) = delete;

    /*
     * Producers
     */
    private:
    // Producer function type
    using ProducerFunction = std::function<std::shared_ptr<ProcedureNode>()>;
    // Typedefs
    using ProcedureNodeRegistryData = std::pair<ProducerFunction, std::string>;
    using ProcedureNodeInfoData = std::pair<ProcedureNode::NodeType, std::string>;
    // Producers for all node types
    std::map<ProcedureNode::NodeType, ProcedureNodeRegistryData> producers_;
    // Categorised map of nodes
    std::map<std::string, std::vector<ProcedureNodeRegistry::ProcedureNodeInfoData>> categories_;

    private:
    // Register producer for node
    template <class N> void registerProducer(ProcedureNode::NodeType nodeType, std::string brief, std::string category = "")
    {
        // Check for duplicate node type
        if (producers_.find(nodeType) != producers_.end())
            throw(std::runtime_error(
                fmt::format("A node producer for type '{}' already exists.\n", ProcedureNode::nodeTypes().keyword(nodeType))));

        producers_.emplace(nodeType, ProcedureNodeRegistryData([]() { return std::make_shared<N>(); }, brief));

        if (!category.empty())
            categories_[category].emplace_back(ProcedureNodeInfoData(nodeType, brief));
    }
    // Produce node of specified type
    std::shared_ptr<ProcedureNode> produce(ProcedureNode::NodeType nodeType) const;
    // Return categorised map of nodes
    const std::map<std::string, std::vector<ProcedureNodeRegistry::ProcedureNodeInfoData>> &categories() const;

    /*
     * Instance
     */
    private:
    // Return the producer instance
    static const ProcedureNodeRegistry &instance();

    /*
     * ProcedureNode Management
     */
    public:
    // Return category map
    static const std::map<std::string, std::vector<ProcedureNodeRegistry::ProcedureNodeInfoData>> &categoryMap();
    // Create new node
    static std::shared_ptr<ProcedureNode> create(ProcedureNode::NodeType nodeType);
};
