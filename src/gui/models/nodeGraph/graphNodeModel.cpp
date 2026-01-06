// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "graphNodeModel.h"
#include "graphModel.h"
#include <qvariant.h>

GraphNodeModel::GraphNodeModel(GraphModel *parent) : parent_(parent) {}
GraphNodeModel::GraphNodeModel(const GraphNodeModel &other) : parent_(other.parent_) {}

enum Role
{
    NAME = 0,
    POSX,
    POSY,
    TYPE,
    ICON,
    INPUTS,
    OUTPUTS,
    OPTIONS,
    INNER_GRAPH,
};

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
    roles[Qt::UserRole + (int)NAME] = "name";
    roles[Qt::UserRole + (int)POSX] = "posX";
    roles[Qt::UserRole + (int)POSY] = "posY";
    roles[Qt::UserRole + (int)TYPE] = "type";
    roles[Qt::UserRole + (int)ICON] = "icon";
    roles[Qt::UserRole + (int)INPUTS] = "inputs";
    roles[Qt::UserRole + (int)OUTPUTS] = "outputs";
    roles[Qt::UserRole + (int)OPTIONS] = "options";
    roles[Qt::UserRole + (int)INNER_GRAPH] = "inner_graph";
    return roles;
}

// Data accessor (required by QAbstractListModel)
QVariant GraphNodeModel::data(const QModelIndex &index, int role) const
{
    auto &item = parent_->wrapped_[index.row()];
    switch (role - Qt::UserRole)
    {
        case NAME:
            return QString::fromStdString(std::string(item.rawValue().name()));
        case POSX:
            return item.rawValue().x;
        case POSY:
            return item.rawValue().y;
        case TYPE:
            return QString::fromStdString(std::string(item.rawValue().type()));
        case ICON:
            return QString::fromStdString(std::format("qrc:/IconsModule/nodes/{}.svg", item.rawValue().type()));
        case INPUTS:
            return QVariant::fromValue(item.inputs.get());
        case OUTPUTS:
            return QVariant::fromValue(item.outputs.get());
        case OPTIONS:
            return QVariant::fromValue(item.options.get());
        case INNER_GRAPH:
            return item.hasInner();
    }
    return {};
}

bool GraphNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &item = parent_->wrapped_[index.row()];
    switch (role - Qt::UserRole)
    {
        case NAME:
            item.rawValue().setName(value.toString().toStdString());
            return true;
        case POSX:
            item.rawValue().x = value.toInt();
            Q_EMIT updatePosition(index.row());
            return true;
        case POSY:
            item.rawValue().y = value.toInt();
            Q_EMIT updatePosition(index.row());
            return true;
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
