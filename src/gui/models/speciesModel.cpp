// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/speciesModel.h"

// Set source Species data
void SpeciesModel::setData(const std::vector<std::unique_ptr<Species>> &species)
{
    beginResetModel();
    species_ = species;
    endResetModel();
}

// Set vector containing checked items
void SpeciesModel::setCheckStateData(std::vector<const Species *> &checkedItemsVector)
{
    beginResetModel();
    checkedItems_ = checkedItemsVector;
    endResetModel();
}

const Species *SpeciesModel::rawData(const QModelIndex &index) const
{
    assert(species_);
    return species_->get()[index.row()].get();
}

// Refresh model data
void SpeciesModel::reset()
{
    beginResetModel();
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int SpeciesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return species_ ? species_->get().size() : 0;
}

QVariant SpeciesModel::data(const QModelIndex &index, int role) const
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
        case (static_cast<unsigned int>(SpeciesUserRole::RawData)):
            return QVariant::fromValue(rawData(index));
        case (static_cast<unsigned int>(SpeciesUserRole::BondsCount)):
            return QVariant::fromValue(rawData(index)->nBonds());
        case (static_cast<unsigned int>(SpeciesUserRole::AnglesCount)):
            return QVariant::fromValue(rawData(index)->nAngles());
        case (static_cast<unsigned int>(SpeciesUserRole::TorsionsCount)):
            return QVariant::fromValue(rawData(index)->nTorsions());
        case (static_cast<unsigned int>(SpeciesUserRole::ImpropersCount)):
            return QVariant::fromValue(rawData(index)->nImpropers());
        default:
            return {};
    }
}

bool SpeciesModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

Qt::ItemFlags SpeciesModel::flags(const QModelIndex &index) const
{
    return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                         : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant SpeciesModel::headerData(int section, Qt::Orientation orientation, int role) const
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
