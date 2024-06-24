// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/generatorNodeModel.h"

// Set source node data
void GeneratorNodeModel::setData(const std::vector<ConstNodeRef> &nodes)
{
    beginResetModel();
    nodes_ = nodes;
    endResetModel();
}

// Set node selected function
void GeneratorNodeModel::setNodeSelectedFunction(std::function<bool(ConstNodeRef)> nodeSelectedFunction)
{
    nodeSelectedFunction_ = std::move(nodeSelectedFunction);
}

// Set node selected function
void GeneratorNodeModel::setNodeDeselectedFunction(std::function<bool(ConstNodeRef)> nodeDeselectedFunction)
{
    nodeDeselectedFunction_ = std::move(nodeDeselectedFunction);
}

// Set node presence function
void GeneratorNodeModel::setNodePresenceFunction(std::function<bool(ConstNodeRef)> nodePresenceFunction)
{
    nodePresenceFunction_ = std::move(nodePresenceFunction);
}

ConstNodeRef GeneratorNodeModel::rawData(const QModelIndex &index) const { return nodes_[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int GeneratorNodeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nodes_.size();
}

QVariant GeneratorNodeModel::data(const QModelIndex &index, int role) const
{
    auto node = rawData(index);
    switch (role)
    {
        case (Qt::DisplayRole):
            return QString::fromStdString(std::string(node->name()));
        case (Qt::CheckStateRole):
            if (nodePresenceFunction_)
            {
                return nodePresenceFunction_(node) ? Qt::Unchecked : Qt::Checked;
            }
            else
            {
                return {};
            }
        case (Qt::UserRole):
            return QVariant::fromValue(node);
        default:
            return {};
    }
}

bool GeneratorNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        if (value.value<Qt::CheckState>() == Qt::Checked)
        {
            if (!nodeSelectedFunction_ || !nodeSelectedFunction_(rawData(index)))
                return false;
        }
        else
        {
            if (!nodeDeselectedFunction_ || !nodeSelectedFunction_(rawData(index)))
                return false;
        }

        Q_EMIT dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags GeneratorNodeModel::flags(const QModelIndex &index) const
{
    return nodeSelectedFunction_ && nodeDeselectedFunction_ && nodePresenceFunction_
               ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
               : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant GeneratorNodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            default:
                return {};
        }

    return {};
}
