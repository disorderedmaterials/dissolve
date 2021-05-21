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

// Set vector containing checked items
void ProcedureNodeModel::setCheckStateData(std::vector<const ProcedureNode *> &checkedItemsVector)
{
    beginResetModel();
    checkedItems_ = checkedItemsVector;
    endResetModel();
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
    else if (role == Qt::CheckStateRole && checkedItems_)
        return std::find(checkedItems_->get().begin(), checkedItems_->get().end(), rawData(index)) == checkedItems_->get().end()
                   ? Qt::Unchecked
                   : Qt::Checked;
    else if (role == Qt::UserRole)
        return QVariant::fromValue(rawData(index));

    return QVariant();
}

bool ProcedureNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && checkedItems_)
    {
        auto &xitems = checkedItems_->get();
        if (value.value<Qt::CheckState>() == Qt::Checked)
        {
            if (std::find(xitems.begin(), xitems.end(), rawData(index)) == xitems.end())
                xitems.push_back(rawData(index));
        }
        else
            xitems.erase(std::remove(xitems.begin(), xitems.end(), rawData(index)), xitems.end());

        emit dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags ProcedureNodeModel::flags(const QModelIndex &index) const
{
    return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                         : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ProcedureNodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            default:
                return QVariant();
        }

    return QVariant();
}
