// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/configurationModel.h"

// Set source Configuration data
void ConfigurationModel::setData(const std::vector<std::unique_ptr<Configuration>> &configuration)
{
    beginResetModel();
    configuration_ = configuration;
    endResetModel();
}

// Set vector containing checked items
void ConfigurationModel::setCheckStateData(std::vector<Configuration *> &checkedItemsVector)
{
    beginResetModel();
    checkedItems_ = checkedItemsVector;
    endResetModel();
}

Configuration *ConfigurationModel::rawData(const QModelIndex &index) const
{
    assert(configuration_);
    return configuration_->get()[index.row()].get();
}

// Refresh model data
void ConfigurationModel::reset()
{
    beginResetModel();
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int ConfigurationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return configuration_ ? configuration_->get().size() : 0;
}

QVariant ConfigurationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

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
        case (static_cast<unsigned int>(ConfigurationUserRole::RawData)):
            return QVariant::fromValue(rawData(index));
        case (static_cast<unsigned int>(ConfigurationUserRole::MoleculesCount)):
            return QVariant::fromValue(rawData(index)->nMolecules());
        case (static_cast<unsigned int>(ConfigurationUserRole::AtomsCount)):
            return QVariant::fromValue(rawData(index)->nAtoms());
        default:
            return {};
    }
}

bool ConfigurationModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

Qt::ItemFlags ConfigurationModel::flags(const QModelIndex &index) const
{
    return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                         : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ConfigurationModel::headerData(int section, Qt::Orientation orientation, int role) const
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
