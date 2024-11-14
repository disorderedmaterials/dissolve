// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generatorGraphModel.h"
#include "expression/variable.h"
#include "gui/models/nodeGraph/instances/all.h"
#include "modules/gr/gr.h"
#include "nodeWrapper.h"
#include "templates/overload_visitor.h"
#include <memory>
#include <variant>

Q_DECLARE_METATYPE(Configuration *)
Q_DECLARE_METATYPE(Generator *)
Q_DECLARE_METATYPE(GeneratorNode *)

GeneratorGraphNode::GeneratorGraphNode(QVariant var) {}

// Dissolve Model Setter
void GeneratorGraphModel::setWorld(DissolveModel *value)
{
    if (!value)
        return;
    world_ = value;
    context_ = &value->dissolve()->coreData();
    auto config = world_->configurationsModel();
    if (config->rowCount() == 0)
        return;
    int index = 50;
    auto nodes = config->rawData(config->index(0))->generator().nodes();
    nodes_.beginInsert(nodes.size());
    for (auto node : nodes)
    {
        QVariant empty = {};
        auto &item = items.emplace_back(empty);
        item.posx = index;
        item.posy = index;
        std::string name = {node->name().begin(), node->name().end()};
        setNodeName(item.rawValue(), name);
        index += 20;
    }
    nodes_.endInsert();
    graphChanged();
}

// Dissolve Model Getter
DissolveModel *GeneratorGraphModel::world() { return world_; }

void GeneratorGraphModel::handleReset()
{
    items.clear();

    int index = 1;
    std::vector<std::pair<int, int>> edges;

    auto configuration = world_->configurationsModel();
    for (auto i = 0; i < configuration->rowCount(); ++i)
    {
        auto config = configuration->rawData(configuration->index(i));
        const auto size = nodes_.rowCount();
        nodes_.beginInsert(config->generator().rootSequence().sequence().size() + 2);
        emplace_back(90 * index++, 60 * index++, config);
        emplace_back(90 * index++, 60 * index++, &config->generator());
        edges.emplace_back(size, size + 1);
        int count = size + 1;
        for (auto genNode : config->generator().rootSequence().sequence())
        {
            emplace_back(90 * index++, 60 * index++, genNode.get());
            edges.emplace_back(count, count + 1);
            count++;
        }
        nodes_.endInsert();
    }
    for (auto [front, back] : edges)
    {
        edges_.addEdge(front, 0, back, 0);
    }
    graphChanged();
}

void GeneratorGraphModel::emplace_back(int x, int y, GeneratorGraphInnerType value)
{
    GeneratorGraphNode temp;
    temp.value = value;
    auto &item = items.emplace_back(temp);
    item.posx = x;
    item.posy = y;
}
