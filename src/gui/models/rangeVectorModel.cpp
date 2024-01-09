// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/rangeVectorModel.h"
#include <QBrush>
#include <fmt/format.h>

// set range data
void RangeVectorModel::setData(std::vector<Range> &ranges)
{
    beginResetModel();
    ranges_ = ranges;
    endResetModel();
}

OptionalReferenceWrapper<Range> RangeVectorModel::getRange(const QModelIndex &index) const
{
    if (!index.isValid() || !ranges_)
    {
        return std::nullopt;
    }

    return ranges_->get()[index.row()];
}

/*
 * QAbstractItemModel overrides
 */

int RangeVectorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (ranges_ ? ranges_->get().size() : 0);
}

int RangeVectorModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant RangeVectorModel::data(const QModelIndex &index, int role) const
{
    if (!ranges_)
    {
        return {};
    }

    auto range = getRange(index);

    switch (role)
    {
        case (Qt::EditRole):
        case (Qt::DisplayRole):
            switch (index.column())
            {
                case (0):
                    return QString::fromStdString(fmt::format("Range {}", index.row() + 1));
                case (1):
                    if (range)
                    {
                        return QString::number(range->get().minimum(), 'g', 5);
                    }
                    else
                    {
                        return {};
                    }
                case (2):
                    if (range)
                    {
                        return QString::number(range->get().maximum(), 'g', 5);
                    }
                    else
                    {
                        return {};
                    }
                default:
                    return {};
            }
        case (Qt::UserRole):
            return QVariant::fromValue(getRange(index));
        default:
            return {};
    }
}

bool RangeVectorModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);
    beginInsertRows(parent, row, row);
    ranges_->get().insert(ranges_->get().begin() + row, Range());
    endInsertRows();
    return true;
}

bool RangeVectorModel::removeRows(int row, int count, const QModelIndex &parent)
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

bool RangeVectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!ranges_ || !value.canConvert<double>())
    {
        return false;
    }

    auto range = getRange(index);
    if (!range)
    {
        return false;
    }
    auto doubleValue = value.toDouble();

    switch (index.column())
    {
        case (1):
            if (range->get().minimum() != doubleValue)
            {
                range->get().setMinimum(doubleValue);

                Q_EMIT dataChanged(index, index.siblingAtColumn(2));
                return true;
            }
            return false;
        case (2):
            if (range->get().maximum() != doubleValue)
            {
                range->get().setMaximum(doubleValue);

                Q_EMIT dataChanged(index, index.siblingAtColumn(1));
                return true;
            }
            return false;
        default:
            return false;
    }

    return false;
}

Qt::ItemFlags RangeVectorModel::flags(const QModelIndex &index) const
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

QVariant RangeVectorModel::headerData(int section, Qt::Orientation orientation, int role) const
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