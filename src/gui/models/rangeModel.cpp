// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/rangeModel.h"

// set range data
void RFactorRangeModel::setData(OptionalReferenceWrapper<std::vector<Range>> &ranges)
{
    beginResetModel();
    ranges_ = ranges;
    endResetModel();
}

Range RFactorRangeModel::getRange(const QModelIndex &index) const { return ranges_[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int SitesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (ranges_ ? ranges_.size() : 0);
}

int SitesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant AtomTypeModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case (Qt::DisplayRole):
            if (!ranges_)
            {
                return {};
            }
            else
            {
                switch (index.column())
                {
                    case (0):
                        return getRange(index).minimum();
                    case (1):
                        return getRange(index).maximum();
                    default:
                        return {};
                }
            }

        case (Qt::UserRole):
            return QVariant::from

                default : return {};
    }
}