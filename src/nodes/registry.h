// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "node.h"
#include <vector>

using NodeProducer = std::function<std::unique_ptr<Node>(Graph *parent)>;
using ProducerMap = std::map<std::string_view, NodeProducer>;

// Registry of all Producible Node Types
class NodeRegistry
{
    public:
    enum Category
    {
        Action,
        Data,
        Export,
        Graphs,
        Import,
        Math,
        Other
    };
    // Return enum option info for Category
    static EnumOptions<Category> category();

    public:
    static std::map<Category, ProducerMap> categoricalProducers_;

    private:
    // Available Node producers
    static ProducerMap producers_;

    private:
    // Remove categories from categorial node producer map, returning a 'flat' producer map
    static ProducerMap decategoriseProducers();
    // Instantiate Node Producers
    static void instantiateNodeProducers();

    public:
    // Check whether the supplied node type is known
    static bool hasNodeType(std::string_view nodeType);
    // Search for the supplied node type, returning all node types
    // which match the selection
    static std::vector<std::string_view> getNodeTypesFuzzy(std::string_view weakNodeType);
    // Produce a node of the given type with the specified Graph parent
    static std::unique_ptr<Node> produce(Graph *parent, std::string_view nodeType);
    // Return producers
    static const std::map<std::string_view, NodeProducer> &producers();
};
