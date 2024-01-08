// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/moduleModel.h"

// Set source Module data
void ModuleModel::setData(const std::vector<Module *> &modules)
{
    beginResetModel();
    modules_ = modules;
    if (checkedItems_)
    {
        auto it =
            std::remove_if(checkedItems_->get().begin(), checkedItems_->get().end(),
                           [&](const auto *m)
                           { return std::find(modules_->get().begin(), modules_->get().end(), m) == modules_->get().end(); });
        if (it != checkedItems_->get().end())
            checkedItems_->get().erase(it, checkedItems_->get().end());
    }
    endResetModel();
}

// Set vector containing checked items
void ModuleModel::setCheckStateData(std::vector<Module *> &checkedItemsVector) { checkedItems_ = checkedItemsVector; }

Module *ModuleModel::rawData(const QModelIndex &index) const { return modules_->get()[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int ModuleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return modules_ ? modules_->get().size() : 0;
}

QVariant ModuleModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case (Qt::DisplayRole):
            return QString::fromStdString(std::string(rawData(index)->name()));
        case (Qt::CheckStateRole):
            if (checkedItems_)
            {
                return std::find(checkedItems_->get().begin(), checkedItems_->get().end(), rawData(index)) ==
                               checkedItems_->get().end()
                           ? Qt::Unchecked
                           : Qt::Checked;
            }
            else
            {
                return {};
            }
        case (Qt::UserRole):
            return QVariant::fromValue(rawData(index));
        default:
            return {};
    }
}

bool ModuleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && checkedItems_)
    {
        auto *m = rawData(index);
        auto it = std::find(checkedItems_->get().begin(), checkedItems_->get().end(), m);
        if (value.value<Qt::CheckState>() == Qt::Checked)
        {
            if (it == checkedItems_->get().end())
                checkedItems_->get().push_back(m);
            else
                return false;
        }
        else
        {
            if (it != checkedItems_->get().end())
                checkedItems_->get().erase(it);
            else
                return false;
        }

        Q_EMIT dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags ModuleModel::flags(const QModelIndex &index) const
{
    return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                         : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ModuleModel::headerData(int section, Qt::Orientation orientation, int role) const
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
