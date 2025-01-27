// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "generatorGraphModel.h"
#include "expression/variable.h"
#include "gui/models/nodeGraph/instances/generatorNode.h"
#include "modules/gr/gr.h"
#include "nodeWrapper.h"
#include <memory>
#include <variant>

// Dissolve Model Setter
void GeneratorGraphModel::setWorld(DissolveModel *value)
{
    if (!value)
        return;
    world_ = value;
    context_ = &value->dissolve().coreData();
    auto config = world_->configurationsModel();
    if (config->rowCount() == 0)
        return;

    // Starting position for new nodes.  This should eventually be
    // replaced by inserting the nodes at random positions and then
    // calling a function to automatically rearrange the nodes.
    int index = 50;

    auto nodes = config->rawData(config->index(0))->generator().nodes();
    nodes_.beginInsert(nodes.size());
    for (auto node : nodes)
    {
        auto &item = items().emplace_back(QVariant::fromValue(node));
        item.posx = index;
        item.posy = index;
        index += 20;
    }
    nodes_.endInsert();
    graphChanged();
}

// Dissolve Model Getter
DissolveModel *GeneratorGraphModel::world() { return world_; }

// Clear model and repopulate data from Dissolve
void GeneratorGraphModel::handleReset()
{
    items().clear();

    int index = 1;
    // A queue to store all of the edges (which will be processed once
    // all of the nodes are added to prevent rerendering).
    std::vector<std::pair<std::string, std::string>> edges;

    auto configuration = world_->configurationsModel();
    for (auto i = 0; i < configuration->rowCount(); ++i)
    {
        auto config = configuration->rawData(configuration->index(i));
        const auto size = nodes_.rowCount();
        nodes_.beginInsert(config->generator().rootSequence().sequence().size() + 2);
        std::string current = "";
        // Connect each generator node in sequence
        for (auto genNode : config->generator().rootSequence().sequence())
        {
            emplace_back(90 * index++, 60 * index++, genNode.get());
            if (!current.empty())
                edges.emplace_back(current, nodeName(genNode.get()));
            current = nodeName(genNode.get());
        }
        nodes_.endInsert();
    }
    for (auto [front, back] : edges)
    {
        edges_.addEdge(front, 0, back, 0);
    }
    graphChanged();
}

// Add a node to the model as a specific location
void GeneratorGraphModel::emplace_back(int x, int y, GeneratorNode *value)
{
    auto &item = items().emplace_back(value);
    item.posx = x;
    item.posy = y;
}
