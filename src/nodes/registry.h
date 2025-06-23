// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "node.h"
#include <string>
#include <vector>

using NodeProducer = std::function<std::unique_ptr<Node>(Graph *parent)>;

// Registry of all Producible Node Types
class NodeRegistry
{
    private:
    // Available Node producers
    static std::map<std::string_view, NodeProducer> producers_;

    private:
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
};
