// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/weightedModuleModel.h"

// Set source Module data
void WeightedModuleModel::setData(const std::vector<Module *> &modules)
{
    beginResetModel();
    modules_ = modules;
    if (weightedItems_)
    {
        auto it = std::remove_if(
            weightedItems_->get().begin(), weightedItems_->get().end(),
            [&](const auto &item)
            { return std::find(modules_->get().begin(), modules_->get().end(), item.first) == modules_->get().end(); });
        if (it != weightedItems_->get().end())
            weightedItems_->get().erase(it, weightedItems_->get().end());
    }
    endResetModel();
}

// Set vector containing selected/weighted items
void WeightedModuleModel::setWeightedItems(std::vector<std::pair<Module *, double>> &weightedItems)
{
    weightedItems_ = weightedItems;
}

Module *WeightedModuleModel::rawData(const QModelIndex &index) const { return modules_->get()[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int WeightedModuleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int WeightedModuleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return modules_ ? modules_->get().size() : 0;
}

QVariant WeightedModuleModel::data(const QModelIndex &index, int role) const
{
    auto module = rawData(index);

    switch (role)
    {
        case (Qt::DisplayRole):
            if (!weightedItems_)
                return {};
            else
                switch (index.column())
                {
                    case (0):
                        return QString::fromStdString(std::string(rawData(index)->name()));
                    case (1):
                    {
                        auto it = std::find_if(weightedItems_->get().begin(), weightedItems_->get().end(),
                                               [module](const auto &item) { return item.first == module; });
                        return it == weightedItems_->get().end() ? "--" : QString::number(it->second);
                    }
                    default:
                        return {};
                }
        case (Qt::CheckStateRole):
            if (!weightedItems_ || index.column() != 0)
                return {};

            return std::find_if(weightedItems_->get().begin(), weightedItems_->get().end(),
                                [module](const auto &item) { return item.first == module; }) == weightedItems_->get().end()
                       ? Qt::Unchecked
                       : Qt::Checked;
        case (Qt::UserRole):
            return QVariant::fromValue(rawData(index));
        default:
            return {};
    }
}

bool WeightedModuleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!weightedItems_)
        return false;

    auto module = rawData(index);
    auto it = std::find_if(weightedItems_->get().begin(), weightedItems_->get().end(),
                           [module](const auto &item) { return item.first == module; });

    switch (index.column())
    {
        case (0):
            if (role == Qt::CheckStateRole)
            {
                if (value.value<Qt::CheckState>() == Qt::Checked)
                {
                    if (it == weightedItems_->get().end())
                        weightedItems_->get().emplace_back(module, 1.0);
                    else
                        return false;
                }
                else
                {
                    if (it != weightedItems_->get().end())
                        weightedItems_->get().erase(it);
                    else
                        return false;
                }

                Q_EMIT dataChanged(index, index.siblingAtColumn(1));

                return true;
            }
            break;
        case (1):
            if (it != weightedItems_->get().end())
                it->second = value.toDouble();
            break;
        default:
            return false;
    }

    return false;
}

Qt::ItemFlags WeightedModuleModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    else
    {

        auto module = rawData(index);
        auto it = std::find_if(weightedItems_->get().begin(), weightedItems_->get().end(),
                               [module](const auto &item) { return item.first == module; });
        return it == weightedItems_->get().end() ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                                                 : Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

QVariant WeightedModuleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Module";
            case 1:
                return "Weight";
            default:
                return {};
        }

    return {};
}
