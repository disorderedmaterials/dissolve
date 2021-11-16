// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/procedureNodeModel.h"

Q_DECLARE_METATYPE(const ProcedureNode *);

// Set source Species data
void ProcedureNodeModel::setData(const std::vector<const ProcedureNode *> &nodes)
{
    beginResetModel();
    nodes_ = nodes;
    endResetModel();
}

// Set node selected function
void ProcedureNodeModel::setNodeSelectedFunction(std::function<bool(const ProcedureNode *)> nodeSelectedFunction)
{
    nodeSelectedFunction_ = std::move(nodeSelectedFunction);
}

// Set node selected function
void ProcedureNodeModel::setNodeDeselectedFunction(std::function<bool(const ProcedureNode *)> nodeDeselectedFunction)
{
    nodeDeselectedFunction_ = std::move(nodeDeselectedFunction);
}

// Set node presence function
void ProcedureNodeModel::setNodePresenceFunction(std::function<bool(const ProcedureNode *)> nodePresenceFunction)
{
    nodePresenceFunction_ = std::move(nodePresenceFunction);
}

const ProcedureNode *ProcedureNodeModel::rawData(const QModelIndex &index) const { return nodes_[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int ProcedureNodeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nodes_.size();
}

QVariant ProcedureNodeModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return QString::fromStdString(std::string(rawData(index)->name()));
    else if (role == Qt::CheckStateRole && nodePresenceFunction_)
        return nodePresenceFunction_(rawData(index)) ? Qt::Unchecked : Qt::Checked;
    else if (role == Qt::UserRole)
        return QVariant::fromValue(rawData(index));

    return {};
}

bool ProcedureNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

        emit dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags ProcedureNodeModel::flags(const QModelIndex &index) const
{
    return nodeSelectedFunction_ && nodeDeselectedFunction_ && nodePresenceFunction_
               ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
               : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ProcedureNodeModel::headerData(int section, Qt::Orientation orientation, int role) const
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
