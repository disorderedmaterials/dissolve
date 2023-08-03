// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/rangeModel.h"
#include <QBrush>
#include <fmt/format.h>

// set range data
void RangeModel::setData(std::vector<Range> &ranges)
{

    beginResetModel();
    ranges_ = ranges;
    endResetModel();
}

Range *RangeModel::getRange(const QModelIndex &index) const { return &ranges_->get()[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int RangeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (ranges_ ? ranges_->get().size() : 0);
}

int RangeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant RangeModel::data(const QModelIndex &index, int role) const
{
    if (!ranges_)
    {
        return {};
    }

    switch (role)
    {
        case (Qt::EditRole):
        case (Qt::DisplayRole):

            switch (index.column())
            {
                case (0):
                    return QString::fromStdString(fmt::format("Range {}", index.row() + 1));
                case (1):
                    return getRange(index)->minimum();
                case (2):
                    return getRange(index)->maximum();
                default:
                    return {};
            }

        case (Qt::UserRole):

            return QVariant::fromValue(getRange(index));

        case (Qt::ForegroundRole):
            if ((index.column() == 1 || index.column() == 2) && getRange(index)->maximum() < getRange(index)->minimum())
            {
                return QBrush(Qt::red);
            }
            else
            {
                return {};
            }

        default:
            return {};
    }
}

bool RangeModel::insertRows(int row, int count, const QModelIndex &parent)
{

    Q_UNUSED(count);

    beginInsertRows(parent, row, row);
    ranges_->get().emplace_back(0.0, 0.0);
    endInsertRows();
    return true;
}

bool RangeModel::removeRows(int row, int count, const QModelIndex &parent)
{

    Q_UNUSED(count);

    if (row >= rowCount() || row < 0)
    {
        return false;
    }

    beginRemoveRows(parent, row, row);
    ranges_->get().erase(ranges_->get().begin() + row);
    endRemoveRows();
    return true;
}

bool RangeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // HEY FUTURE NOELLA WE ARE WORKING ON SETTING THE NEW DATA AND RIGHT NOW STRUGGLING
    // TO CONVERT VALUE TO DOUBLE AND STRING
    if (!ranges_ || !value.canConvert<double>())
    {
        return false;
    }

    auto range = getRange(index);
    auto doubleValue = value.toDouble();

    switch (index.column())
    {

        case (1):
            if (range->minimum() != doubleValue)
            {
                range->setMinimum(doubleValue);

                emit dataChanged(index, index);
                return true;
            }

            return false;

        case (2):
            if (range->maximum() != doubleValue)
            {
                range->setMaximum(doubleValue);

                emit dataChanged(index, index);
                return true;
            }

            return false;

        default:
            return false;
    }

    return false;
}

Qt::ItemFlags RangeModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

QVariant RangeModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section)
        {
            case 0:
                return "Name";
            case 1:
                return "Minimum";
            case 2:
                return "Maximum";
            default:
                return {};
        }

    return {};
}