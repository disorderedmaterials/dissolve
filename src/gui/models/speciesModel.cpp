// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/speciesModel.h"

Q_DECLARE_METATYPE(const Species *);

// Set source Species data
void SpeciesModel::setData(const std::vector<std::unique_ptr<Species>> &species)
{
    beginResetModel();
    species_ = species;
    endResetModel();
}

const Species *SpeciesModel::rawData(const QModelIndex &index) const
{
    assert(species_);
    return species_->get()[index.row()].get();
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
    if (role == Qt::DisplayRole)
        return QString::fromStdString(std::string(rawData(index)->name()));
    else if (role == Qt::UserRole)
        return QVariant::fromValue(rawData(index));

    return QVariant();
}

QVariant SpeciesModel::headerData(int section, Qt::Orientation orientation, int role) const
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
