// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generatorGraphModel.h"
#include "expression/variable.h"
#include "modules/gr/gr.h"
#include "nodeWrapper.h"
#include "templates/overload_visitor.h"
#include <memory>
#include <variant>

Q_DECLARE_METATYPE(Configuration *)

template <> QVariant nodeGetValue<GeneratorGraphNode>(const GeneratorGraphNode value)
{
    return std::visit(overloaded{[](Configuration *arg) { return QVariant::fromValue(arg); },
                                 [](GeneratorGraphNode *arg)
                                 {
                                     if (arg)
                                         return nodeGetValue<GeneratorGraphNode>(*arg);
                                     QVariant empty;
                                     return empty;
                                 }},
                      value.value);
}

template <> std::string nodeTypeName<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit(
        overloaded{[](Configuration *arg) { return "Configuration"; }, [](GeneratorGraphNode *arg) { return "ptr"; }},
        value.value);
}

template <>
bool nodeConnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination,
                                     int destinationIndex)
{
    return true;
}

template <>
bool nodeConnectable<GeneratorGraphNode>(const GeneratorGraphNode &source, int sourceIndex,
                                         const GeneratorGraphNode &destination, int destinationIndex)
{
    return false;
}

template <>
bool nodeDisconnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination,
                                        int destinationIndex)
{
    return true;
}

template <> std::string nodeTypeIcon<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit(overloaded{[](Configuration *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; },
                                 [](GeneratorGraphNode *arg)
                                 { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; }},
                      value.value);
}

template <> std::string nodeName<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit(overloaded{[](Configuration *arg)
                                 {
                                     if (!arg)
                                         return std::string("Null");
                                     std::string s = {arg->niceName().begin(), arg->name().end()};
                                     return s;
                                 },
                                 [](GeneratorGraphNode *arg)
                                 {
                                     if (arg)
                                         return nodeName(*arg);
                                     else
                                         return std::string("Unlinked");
                                 }},
                      value.value);
    return "Generator Name";
}

template <> void setNodeName<GeneratorGraphNode>(GeneratorGraphNode &value, const std::string name) {}

GeneratorGraphNode::GeneratorGraphNode(QVariant var)
{
    if (var.isNull())
        value.emplace<GeneratorGraphNode *>(nullptr);
    else
        value.emplace<Configuration *>(var.value<Configuration *>());
}

template <> bool nodeSetData<GeneratorGraphNode>(GeneratorGraphNode &item, const QVariant &value, int role)
{
    switch (role)
    {
        case 1:
            std::get<Configuration *>(item.value)->setTemperature(value.toDouble());
            return true;
        default:
            return false;
    }
}

template <> QVariant nodeData(const GeneratorGraphNode &item, int role)
{
    switch (role)
    {
        case 0:
            return nodeGetValue(item);
        case 1:
            return std::get<Configuration *>(item.value)->temperature();
        default:
            return {};
    }
}

template <> QHash<int, QByteArray> &nodeRoleNames<GeneratorGraphNode>(QHash<int, QByteArray> &roles, int index)
{
    roles[index++] = "value";
    roles[index++] = "temperature";
    return roles;
}

// Dissolve Model Setter
void GeneratorGraphModel::setWorld(ConfigurationModel *value)
{
    if (!value)
        return;
    world_ = value;
    if (world_->rowCount() == 0)
        return;
    int index = 50;
    auto nodes = world_->rawData(world_->index(0))->generator().nodes();
    // nodes_.beginInsertRows({}, items.size(), items.size() + nodes.size());
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
    // nodes_.endInsertRows();
    graphChanged();
}

// Dissolve Model Getter
ConfigurationModel *GeneratorGraphModel::world() { return world_; }

void GeneratorGraphModel::handleReset()
{
    items.clear();

    int index = 50;
    for (auto i = 0; i < world_->rowCount(); ++i)
    {
        auto config = world_->rawData(world_->index(i));
        QVariant variant;
        variant.setValue(config);
        emplace_back(index, index, variant);
    }
}
