// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesSiteModel.h"

SpeciesSiteModel::SpeciesSiteModel(OptionalReferenceWrapper<std::vector<std::unique_ptr<SpeciesSite>>> sites,
                                   OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItems)
{
    if (sites)
        setData(*sites);
    if (checkedItems)
        setCheckStateData(*checkedItems);
}

// Set source SpeciesSite data
void SpeciesSiteModel::setData(OptionalReferenceWrapper<std::vector<std::unique_ptr<SpeciesSite>>> sites)
{
    beginResetModel();
    sites_ = sites;
    endResetModel();
}

// Set vector containing checked items
void SpeciesSiteModel::setCheckStateData(OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItemsVector)
{
    beginResetModel();
    checkedItems_ = checkedItemsVector;
    endResetModel();
}

SpeciesSite *SpeciesSiteModel::rawData(const QModelIndex &index) const
{
    assert(sites_);

    if (index.isValid())
        return sites_->get()[index.row()].get();

    return nullptr;
}

/*
 * QAbstractItemModel overrides
 */

int SpeciesSiteModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return sites_ ? sites_->get().size() : 0;
}

QVariant SpeciesSiteModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case (Qt::DisplayRole):
        case (Qt::EditRole):
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

bool SpeciesSiteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        rawData(index)->setName(value.toString().toStdString());

        Q_EMIT dataChanged(index, index);

        return true;
    }
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

Qt::ItemFlags SpeciesSiteModel::flags(const QModelIndex &index) const
{
    return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                         : Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant SpeciesSiteModel::headerData(int section, Qt::Orientation orientation, int role) const
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
