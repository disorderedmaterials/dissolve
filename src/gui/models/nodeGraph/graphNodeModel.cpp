// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphNodeModel.h"
#include "gui/models/nodeGraph/graphModel.h"

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
            return parent_->typeName(item.rawValue()).c_str();
        case 4:
            return parent_->typeIcon(item.rawValue()).c_str();
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
