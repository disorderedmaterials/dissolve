// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/graphModel.h"
#include <qnamespace.h>
#include <qvariant.h>

GraphEdgeModel::GraphEdgeModel(GraphModel *parent) : parent_(parent) {}
GraphEdgeModel::GraphEdgeModel(const GraphEdgeModel &other) : parent_(other.parent_) {}

int GraphEdgeModel::rowCount(const QModelIndex &parent) const { return 1; }

QHash<int, QByteArray> GraphEdgeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "source";
    roles[Qt::UserRole + 1] = "sourceIndex";
    roles[Qt::UserRole + 2] = "destination";
    roles[Qt::UserRole + 3] = "destIndex";
    return roles;
}

QVariant GraphEdgeModel::data(const QModelIndex &index, int role) const
{

    switch (role - Qt::UserRole)
    {
        case 0:
            return 0;
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 0;
        default:
            return {};
    }
}

GraphEdgeModel &GraphEdgeModel::operator=(const GraphEdgeModel &other)
{
    parent_ = other.parent_;
    return *this;
}

bool GraphEdgeModel::operator!=(const GraphEdgeModel &other) { return parent_ != other.parent_; }

// Graph Node Model

GraphNodeModel::GraphNodeModel(GraphModel *parent) : parent_(parent) {}
GraphNodeModel::GraphNodeModel(const GraphNodeModel &other) : parent_(other.parent_) {}

bool GraphNodeModel::operator!=(const GraphNodeModel &other) { return &parent_ != &other.parent_; }

int GraphNodeModel::rowCount(const QModelIndex &parent) const { return parent_->items.size(); }

QHash<int, QByteArray> GraphNodeModel::roleNames() const
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

QVariant GraphNodeModel::data(const QModelIndex &index, int role) const
{
    auto &item = parent_->items.at(index.row());
    switch (role - Qt::UserRole)
    {
        case 0:
            return item.name.c_str();
        case 1:
            return item.posx;
        case 2:
            return item.posy;
        case 3:
            return item.type.c_str();
        case 4:
            return item.icon.c_str();
        case 5:
            return item.value();
    }
    return {};
}

GraphNodeModel &GraphNodeModel::operator=(const GraphNodeModel &other)
{
    parent_ = other.parent_;
    return *this;
}

// Graph Model

GraphModel::GraphModel() : nodes_(this), edges_(this)
{
    auto &first = items.emplace_back(7.5);
    first.name = "Source";
    first.icon = "file:/home/adam/Code/dissolve/src/gui/icons/open.svg";
    first.posx = 100;
    first.posy = 300;
    first.type = "number";

    auto &second = items.emplace_back(12.5);
    second.name = "Destination";
    second.icon = "file:/home/adam/Code/dissolve/src/gui/icons/options.svg";
    second.posx = 600;
    second.posy = 400;
    second.type = "number";
}

GraphEdgeModel *GraphModel::edges() { return &edges_; }
GraphNodeModel *GraphModel::nodes() { return &nodes_; }

NodeWrapper::NodeWrapper(nodeValue value) : value_(value) {}
QVariant NodeWrapper::value() { return getValue(value_); }

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

double getValue(nodeValue value)
{
    return std::visit(overloaded{[](double arg) { return arg; }, [](double *arg) { return *arg; }}, value);
}
