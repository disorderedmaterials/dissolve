// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "exampleGraphModel.h"
#include "templates/overload_visitor.h"
#include <variant>

template <> QVariant nodeGetValue<nodeValue>(const nodeValue value)
{
    return std::visit(overloaded{[](double arg) { return QVariant::fromValue(arg); },
                                 [](nodeValue *arg)
                                 {
                                     if (arg)
                                         return nodeGetValue<nodeValue>(*arg);
                                     QVariant empty;
                                     return empty;
                                 }},
                      value.value);
}

template <> std::string nodeTypeName<nodeValue>(const nodeValue &value)
{
    return std::visit(overloaded{[](double arg) { return "number"; }, [](nodeValue *arg) { return "ptr"; }}, value.value);
}

template <> std::string nodeTypeIcon<nodeValue>(const nodeValue &value)
{
    return std::visit(overloaded{[](double arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; },
                                 [](nodeValue *arg) { return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg"; }},
                      value.value);
}

template <> std::string nodeName<nodeValue>(const nodeValue &value) { return value.name; }

template <> void setNodeName<nodeValue>(nodeValue &value, const std::string name) { value.name = name; }

template <> bool nodeConnect<nodeValue>(nodeValue &source, int sourceIndex, nodeValue &destionation, int destinationIndex)
{
    destionation.value = &source;
    return true;
}

template <>
bool nodeConnectable<nodeValue>(const nodeValue &source, int sourceIndex, const nodeValue &destination, int destinationIndex)
{
    return std::holds_alternative<double>(source.value) && std::holds_alternative<nodeValue *>(destination.value);
}

template <> bool nodeDisconnect<nodeValue>(nodeValue &source, int sourceIndex, nodeValue &destination, int destinationIndex)
{
    destination.value = nullptr;
    return true;
}

template <> QHash<int, QByteArray> nodeRoleNames<nodeValue>()
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "name";
    roles[Qt::UserRole + 1] = "posX";
    roles[Qt::UserRole + 2] = "posY";
    roles[Qt::UserRole + 3] = "type";
    roles[Qt::UserRole + 4] = "icon";
    roles[Qt::UserRole + 5] = "value";
    return roles;
}

template <> QVariant nodeData<nodeValue>(const NodeWrapper<nodeValue> &item, int role)
{
    switch (role - Qt::UserRole)
    {
        case 0:
            return nodeName(item.rawValue()).c_str();
        case 1:
            return item.posx;
        case 2:
            return item.posy;
        case 3:
            return nodeTypeName(item.rawValue()).c_str();
        case 4:
            return nodeTypeIcon(item.rawValue()).c_str();
        case 5:
            return item.value();
    }
    return {};
}

template <> bool nodeSetData<nodeValue>(NodeWrapper<nodeValue> &item, const QVariant &value, int role)
{
    switch (role - Qt::UserRole)
    {
        case 0:
            setNodeName(item.rawValue(), value.toString().toStdString());
            return true;
        case 1:
            item.posx = value.toInt();
            return true;
        case 2:
            item.posy = value.toInt();
            return true;
        default:
            return false;
    }
    return false;
}

nodeValue::nodeValue(QVariant var)
{
    if (var.isNull())
        value = nullptr;
    else
        value = var.toDouble();
}
