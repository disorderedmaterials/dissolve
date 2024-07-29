// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include <map>

// Forward Declarations
class KeywordWidgetBase;
class QWidget;

// GeneratorNode Registry
class GeneratorNodeRegistry
{
    private:
    GeneratorNodeRegistry();

    public:
    GeneratorNodeRegistry(const GeneratorNodeRegistry &) = delete;
    GeneratorNodeRegistry(GeneratorNodeRegistry &&) = delete;
    GeneratorNodeRegistry &operator=(const GeneratorNodeRegistry &) = delete;
    GeneratorNodeRegistry &operator=(GeneratorNodeRegistry &&) = delete;

    /*
     * Producers
     */
    private:
    // Producer function type
    using ProducerFunction = std::function<std::shared_ptr<GeneratorNode>()>;
    // Typedefs
    using GeneratorNodeRegistryData = std::pair<ProducerFunction, std::string>;
    using GeneratorNodeInfoData = std::pair<GeneratorNode::NodeType, std::string>;
    // Producers for all node types
    std::map<GeneratorNode::NodeType, GeneratorNodeRegistryData> producers_;
    // Categorised map of nodes
    std::map<std::string, std::vector<GeneratorNodeRegistry::GeneratorNodeInfoData>> categories_;

    private:
    // Register producer for node
    template <class N> void registerProducer(GeneratorNode::NodeType nodeType, std::string brief, std::string category = "")
    {
        // Check for duplicate node type
        if (producers_.find(nodeType) != producers_.end())
            throw(std::runtime_error(
                fmt::format("A node producer for type '{}' already exists.\n", GeneratorNode::nodeTypes().keyword(nodeType))));

        producers_.emplace(nodeType, GeneratorNodeRegistryData([]() { return std::make_shared<N>(); }, brief));

        if (!category.empty())
            categories_[category].emplace_back(GeneratorNodeInfoData(nodeType, brief));
    }
    // Produce node of specified type
    std::shared_ptr<GeneratorNode> produce(GeneratorNode::NodeType nodeType) const;
    // Return categorised map of nodes
    const std::map<std::string, std::vector<GeneratorNodeRegistry::GeneratorNodeInfoData>> &categories() const;

    /*
     * Instance
     */
    private:
    // Return the producer instance
    static const GeneratorNodeRegistry &instance();

    /*
     * GeneratorNode Management
     */
    public:
    // Return category map
    static const std::map<std::string, std::vector<GeneratorNodeRegistry::GeneratorNodeInfoData>> &categoryMap();
    // Create new node
    static std::shared_ptr<GeneratorNode> create(GeneratorNode::NodeType nodeType);
};
