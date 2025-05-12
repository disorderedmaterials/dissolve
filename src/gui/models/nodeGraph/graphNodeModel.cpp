// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "graphModel.h"
#include "graphNodeModel.h"
#include <qvariant.h>

GraphNodeModel::GraphNodeModel(GraphModel *parent) : parent_(parent) {}
GraphNodeModel::GraphNodeModel(const GraphNodeModel &other) : parent_(other.parent_) {}

GraphNodeModel &GraphNodeModel::operator=(const GraphNodeModel &other)
{
    parent_ = other.parent_;
    return *this;
}

bool GraphNodeModel::operator!=(const GraphNodeModel &other) { return &parent_ != &other.parent_; }

void GraphNodeModel::updateGraph()
{
    beginResetModel();
    endResetModel();
}

// Number of nodes (required by QAbstractListModel)
int GraphNodeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent_->graph())
        return 0;
    return parent_->graph()->nodes().size();
}

// Labels for QML roles (required by QAbstractListModel)
QHash<int, QByteArray> GraphNodeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "name";
    roles[Qt::UserRole + 1] = "posX";
    roles[Qt::UserRole + 2] = "posY";
    roles[Qt::UserRole + 3] = "type";
    roles[Qt::UserRole + 4] = "icon";
    return roles;
}

// Data accessor (required by QAbstractListModel)
QVariant GraphNodeModel::data(const QModelIndex &index, int role) const
{
    auto &item = parent_->wrapped_[index.row()];
    switch (role - Qt::UserRole)
    {
        case 0:
            return QString::fromStdString(std::string(item.rawValue().name()));
        case 1:
            return item.posx;
        case 2:
            return item.posy;
        case 3:
            return QString::fromStdString(std::string(item.rawValue().type()));
        case 4:
            return QString::fromStdString(std::format("qrc:/Dissolve/icons/nodes/{}.svg", item.rawValue().type()));
    }
    return {};
}

bool GraphNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &item = parent_->wrapped_[index.row()];
    switch (role - Qt::UserRole)
    {
        case 0:
            item.rawValue().setName(value.toString().toStdString());
            return true;
        case 1:
            item.posx = value.toInt();
            return true;
        case 2:
            item.posy = value.toInt();
            return true;
            // default:
            //     return nodeSetData(item.rawValue(), value, role - Qt::UserRole - ownedRoles);
    }
    return false;
}

// Must call *before* inserting new elements.  The count is the number of elements that will be inserted
void GraphNodeModel::beginInsert(int count)
{
    beginInsertRows({}, parent_->graph()->nodes().size(), parent_->graph()->nodes().size() + count);
}

// Must call *after* inserting new elements
void GraphNodeModel::endInsert() { endInsertRows(); }
