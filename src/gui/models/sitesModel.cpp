// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/sitesModel.h"

SitesModel::SitesModel(OptionalReferenceWrapper<const std::vector<std::unique_ptr<Species>>> species,
                       OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItems)
{
    if (species)
        setData(*species);
    if (checkedItems)
        setCheckStateData(*checkedItems);
}

// Set source SpeciesSite data
void SitesModel::setData(const std::vector<std::unique_ptr<Species>> &species)
{
    beginResetModel();
    species_ = species;
    endResetModel();
}

// Set vector containing checked items
void SitesModel::setCheckStateData(std::vector<const SpeciesSite *> &checkedItemsVector)
{
    beginResetModel();
    checkedItems_ = checkedItemsVector;
    endResetModel();
}

SpeciesSite *SitesModel::rawData(const QModelIndex &index) const
{
    assert(species_);

    if (!index.isValid() || !index.parent().isValid())
        return nullptr;

    // Get species based on parent row index
    const auto &sp = species_->get()[index.parent().row()];
    return sp->sites()[index.row()].get();
}

/*
 * QAbstractItemModel overrides
 */

int SitesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!species_)
        return 0;

    if (parent.isValid())
    {
        const auto &sp = species_->get()[parent.row()];
        return sp->nSites();
    }

    return species_->get().size();
}

int SitesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

bool SitesModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QModelIndex SitesModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

QModelIndex SitesModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = parent.row() + 1;
    else
        return {};

    return createIndex(row, column, child);
}

QVariant SitesModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid() && index.column() == 1)
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
        }
    }
    else if (!index.parent().isValid() && index.column() == 0)
    {
        // Get the indicated species
        const auto &sp = species_->get()[index.row()];
        if (role == Qt::DisplayRole)
            return QString::fromStdString(std::string(sp->name()));
    }

    return QVariant();
}

bool SitesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.parent().isValid())
        return false;
    else if (role == Qt::CheckStateRole && checkedItems_)
    {
        auto &xitems = checkedItems_->get();
        if (value.value<Qt::CheckState>() == Qt::Checked)
        {
            if (std::find(xitems.begin(), xitems.end(), rawData(index)) == xitems.end())
                xitems.push_back(rawData(index));
        }
        else
            xitems.erase(std::remove(xitems.begin(), xitems.end(), rawData(index)), xitems.end());

        Q_EMIT dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags SitesModel::flags(const QModelIndex &index) const
{
    return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                         : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant SitesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Species";
            case 1:
                return "Site";
            default:
                return QVariant();
        }

    return QVariant();
}
