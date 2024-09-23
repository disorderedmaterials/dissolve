// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/graphModel.h"
#include <qnamespace.h>

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

int GraphNodeModel::rowCount(const QModelIndex &parent) const { return 2; }

QHash<int, QByteArray> GraphNodeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "name";
    roles[Qt::UserRole + 1] = "posX";
    roles[Qt::UserRole + 2] = "posY";
    roles[Qt::UserRole + 3] = "type";
    roles[Qt::UserRole + 4] = "icon";
    roles[Qt::UserRole + 5] = "value";
    roles[Qt::UserRole + 6] = "px";
    roles[Qt::UserRole + 7] = "py";
    return roles;
}

QVariant GraphNodeModel::data(const QModelIndex &index, int role) const
{
    switch (role - Qt::UserRole)
    {
        case 0:
            if (index.row() == 0)
            {
                return "Source";
            }
            else
            {
                return "Destination";
            }
        case 1:
            if (index.row() == 0)
            {
                return 100;
            }
            else
            {
                return 600;
            }
        case 2:
            if (index.row() == 0)
            {
                return 300;
            }
            else
            {
                return 400;
            }
        case 3:
            if (index.row() == 0)
            {
                return "int";
            }
            else
            {
                return "point2d";
            }
        case 4:
            if (index.row() == 0)
            {
                return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg";
            }
            else
            {
                return "file:/home/adam/Code/dissolve/src/gui/icons/options.svg";
            }
        case 5:
            return 7;
        case 6:
            return 3.5;
        case 7:
            return 7.5;
    }
    return {};
}

GraphNodeModel &GraphNodeModel::operator=(const GraphNodeModel &other)
{
    parent_ = other.parent_;
    return *this;
}

// Graph Model

GraphModel::GraphModel() : nodes_(this), edges_(this) {}

GraphEdgeModel *GraphModel::edges() { return &edges_; }
GraphNodeModel *GraphModel::nodes() { return &nodes_; }

IntNode::IntNode() {}
