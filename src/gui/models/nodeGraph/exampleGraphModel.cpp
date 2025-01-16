// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "exampleGraphModel.h"
#include "nodeWrapper.h"
#include <type_traits>
#include <variant>

// The value of the node
QVariant nodeGetValue(const nodeValue &value)
{
    return std::visit(
        [](auto arg) -> QVariant
        {
            if constexpr (std::is_same_v<decltype(arg), double>)
            {
                return QVariant::fromValue(arg);
            }
            else
            {
                if (arg)
                    return nodeGetValue(*arg);
                return {};
            }
        },
        value.value);
};

// The name of the type (for delegate dispatch)
std::string nodeTypeName(const nodeValue &value)
{
    return std::visit(
        [](auto arg) -> std::string
        {
            if constexpr (std::is_same_v<decltype(arg), double>)
            {
                return "number";
            }
            else
            {
                return "ptr";
            }
        },
        value.value);
}

// The path to the icon for the node
std::string nodeTypeIcon(const nodeValue &value)
{
    return std::visit(
        [](auto arg) -> std::string
        {
            if constexpr (std::is_same_v<decltype(arg), double>)
            {
                return "qrc:/Dissolve/icons/open.svg";
            }
            else
            {
                return "qrc:/Dissolve/icons/cross.svg";
            }
        },
        value.value);
}

// The title of the node
std::string nodeName(const nodeValue &value) { return value.name; }

// Change the title of the node
void setNodeName(nodeValue &value, const std::string name) { value.name = name; }

// Link an indexed position on the source to an indexed position on the destination
bool nodeConnect(nodeValue &source, int sourceIndex, nodeValue &destionation, int destinationIndex)
{
    destionation.value = &source;
    return true;
}

// Confirm that a connection is possible (e.g. types match and index isn't already connected)

bool nodeConnectable(const nodeValue &source, int sourceIndex, const nodeValue &destination, int destinationIndex)
{
    return std::holds_alternative<double>(source.value) && std::holds_alternative<nodeValue *>(destination.value);
}

// Unlink an indexed position on the source to an indexed position on the destination
bool nodeDisconnect(nodeValue &source, int sourceIndex, nodeValue &destination, int destinationIndex)
{
    destination.value = nullptr;
    return true;
}

// Append the roles for the type onto the QHash
QHash<int, QByteArray> &nodeRoleNames(Proxy<nodeValue> proxy, QHash<int, QByteArray> &roles)
{
    const auto base = Qt::UserRole + GraphNodeModelBase::ownedRoles;
    roles[base] = "value";
    return roles;
}

// Get a specific piece of information from a node by index
QVariant nodeData(const nodeValue &item, int role)
{
    switch (role)
    {
        case 0:
            return nodeGetValue(item);
        default:
            return {};
    }
}

// Set a specific piece of information from a node by index
bool nodeSetData(nodeValue &item, const QVariant &value, int role) { return false; }

// Delete the node
bool nodeDelete(nodeValue &value, Proxy<nodeValue> &context) { return true; }

// Create node from variant
nodeValue::nodeValue(QVariant var)
{
    if (var.isNull())
        value = nullptr;
    else
        value = var.toDouble();
}
