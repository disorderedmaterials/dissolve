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
Q_DECLARE_METATYPE(Generator *)

template <> QVariant nodeGetValue<GeneratorGraphNode>(const GeneratorGraphNode value)
{
    return std::visit(overloaded{[](Configuration *arg) { return QVariant::fromValue(arg); },
                                 [](Generator *arg) { return QVariant::fromValue(arg); },
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
    return std::visit(overloaded{[](Configuration *arg) { return "Configuration"; }, [](Generator *arg) { return "Generator"; },
                                 [](GeneratorGraphNode *arg) { return "ptr"; }},
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
    return std::visit(
        overloaded{[](Configuration *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/configuration.svg"; },
                   [](Generator *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/generator.svg"; },
                   [](GeneratorGraphNode *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; }},
        value.value);
}

template <> std::string nodeName<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit(
        overloaded{
            [](Configuration *arg)
            {
                if (!arg)
                    return std::string("Null");
                std::string s = {arg->niceName().begin(), arg->niceName().end()};
                return s;
            },
            [](Generator *arg) -> std::string { return std::string("Generator"); },
        },
        value.value);
    return "Generator Name";
}

template <> void setNodeName<GeneratorGraphNode>(GeneratorGraphNode &value, const std::string name) {}

GeneratorGraphNode::GeneratorGraphNode(QVariant var) {}

template <> bool nodeSetData<GeneratorGraphNode>(GeneratorGraphNode &item, const QVariant &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    switch (role)
    {
        case names::Temperature:
            std::get<Configuration *>(item.value)->setTemperature(value.toDouble());
            return true;
        default:
            return false;
    }
}

template <> QVariant nodeData(const GeneratorGraphNode &item, int role)
{
    std::cout << "Item Index: " << item.value.index() << "\t" << role << std::endl;
    using names = GeneratorGraphModel::PropertyIndex;
    return std::visit(
        overloaded{
            [role](Configuration *arg) -> QVariant
            {
                if (!arg)
                    return {};
                switch (role)
                {
                    case names::Value:
                        return QVariant::fromValue(arg);
                    case names::Temperature:
                        return arg->temperature();
                    case names::AtomicDensity:
                        return arg->atomicDensity().value_or(0.0);
                    default:
                        return {};
                }
            },
            [role](Generator *arg) -> QVariant
            {
                if (!arg)
                    return {};
                switch (role)
                {
                    case names::Value:
                        return QVariant::fromValue(arg);
                    default:
                        return {};
                }
            },
        },
        item.value);
}

template <> QHash<int, QByteArray> &nodeRoleNames<GeneratorGraphNode>(QHash<int, QByteArray> &roles)
{
    auto base = Qt::UserRole + GraphNodeModel<GeneratorGraphNode>::ownedRoles;
    using names = GeneratorGraphModel::PropertyIndex;
    roles[base + names::Value] = "value";
    roles[base + names::Temperature] = "temperature";
    roles[base + names::AtomicDensity] = "atomicDensity";
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
ConfigurationModel *GeneratorGraphModel::world() { return world_; }

void GeneratorGraphModel::handleReset()
{
    items.clear();

    int index = 50;
    nodes_.beginInsert();
    for (auto i = 0; i < world_->rowCount(); ++i)
    {
        auto config = world_->rawData(world_->index(i));
        std::cout << "Config:\t" << config << std::endl;
        emplace_back(index, index, config);
        std::cout << "Generator:\t" << &config->generator() << std::endl;
        emplace_back(index, index, &config->generator());
    }
    nodes_.endInsert();
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
