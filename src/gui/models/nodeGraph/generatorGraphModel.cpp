// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generatorGraphModel.h"
#include "modules/gr/gr.h"
#include "nodeWrapper.h"
#include "templates/overload_visitor.h"
#include <memory>
#include <variant>

template <> QVariant nodeGetValue<GeneratorGraphNode>(const GeneratorGraphNode value)
{
    return std::visit(overloaded{[](AddGeneratorNode *arg) { return QVariant::fromValue(arg); },
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
        overloaded{[](AddGeneratorNode *arg) { return "AddGeneratorNode"; }, [](GeneratorGraphNode *arg) { return "ptr"; }},
        value.value);
}

template <>
bool nodeConnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destionation,
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
    return std::visit(overloaded{[](AddGeneratorNode *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; },
                                 [](GeneratorGraphNode *arg)
                                 { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; }},
                      value.value);
}

template <> std::string nodeName<GeneratorGraphNode>(const GeneratorGraphNode &value) { return "Generator Name"; }

template <> void setNodeName<GeneratorGraphNode>(GeneratorGraphNode &value, const std::string name) {}

GeneratorGraphNode::GeneratorGraphNode(QVariant var)
{
    if (var.isNull())
        value.emplace<GeneratorGraphNode *>(nullptr);
    else
        value.emplace<AddGeneratorNode *>(nullptr);
}

template <> bool nodeSetData<GeneratorGraphNode>(NodeWrapper<GeneratorGraphNode> &, const QVariant &value, int role)
{
    return false;
}

template <> QVariant nodeData(const NodeWrapper<GeneratorGraphNode> &item, int role)
{
    switch (role)
    {
        case 0:
            return item.value();
        default:
            return {};
    }
}

template <> QHash<int, QByteArray> &nodeRoleNames<GeneratorGraphNode>(QHash<int, QByteArray> &roles, int index)
{
    roles[index++] = "value";
    return roles;
}
