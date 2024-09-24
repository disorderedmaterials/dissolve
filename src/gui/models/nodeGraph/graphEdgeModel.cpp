// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphEdgeModel.h"
#include "gui/models/nodeGraph/graphModel.h"

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

// Graph Model

GraphModel::GraphModel() : nodes_(this), edges_(this)
{
    auto &first = items.emplace_back(7.5);
    first.name = "Source";
    first.posx = 100;
    first.posy = 300;

    auto &second = items.emplace_back(12.5);
    second.name = "Destination";
    second.posx = 600;
    second.posy = 400;
}
