// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/rangeModel.h"
#include <fmt/format.h>

// set range data
void RangeModel::setData(std::vector<Range> &ranges)
{

    beginResetModel();
    ranges_ = ranges;
    endResetModel();
}

Range RangeModel::getRange(const QModelIndex &index) const { return ranges_->get()[index.row()]; }

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
        case (Qt::DisplayRole):

            switch (index.column())
            {
                case (0):
                    return QString::fromStdString(getRange(index).name());
                case (1):
                    return QString::number(getRange(index).minimum());
                case (2):
                    return QString::number(getRange(index).maximum());
                default:
                    return {};
            }

        case (Qt::UserRole):
            return QVariant::fromValue(getRange(index));

        case (Qt::CheckStateRole):
            if (index.column() != 0)
            {
                return {};
            }

            if (!checkedRanges_)
            {
                return Qt::Unchecked;
            }

            // See if the selected cell's row index is stored in the checkedRanges_ vector
            return std::find(checkedRanges_->get().begin(), checkedRanges_->get().end(), index.row()) ==
                           checkedRanges_->get().end()
                       ? Qt::Unchecked
                       : Qt::Checked;

        default:
            return {};
    }
}

bool RangeModel::insertRows(int row, int count, const QModelIndex &parent)
{

    Q_UNUSED(count);

    beginInsertRows(parent, row, row);
    ++rangeCount_;
    ranges_->get().emplace_back(0.0, 0.0, fmt::format("Range {}", rangeCount_));
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
    if (!ranges_ || !checkedRanges_)
    {
        return false;
    }

    auto range = getRange(index);
    double floatValue = value.value<float>();

    switch (index.column())
    {
        case (0):
            if (role == Qt::CheckStateRole)
            {
                // See if the selected cell's row index is stored in the checkedRanges_ vector
                auto it = find(checkedRanges_->get().begin(), checkedRanges_->get().end(), index.row());

                if (value.value<Qt::CheckState>() == Qt::Checked)
                {
                    // If checked cell is not stored in checkedRanges_, add it
                    if (!checkedRanges_)
                    {
                        checkedRanges_->get().push_back(index.row());
                    }
                    else if (it == checkedRanges_->get().end())
                    {
                        checkedRanges_->get().push_back(index.row());
                    }
                    else
                    {
                        return false;
                    }
                }

                else
                {
                    // If unchecked cell is stored in checkedRanges_, remove it
                    if (!checkedRanges_)
                    {
                        return false;
                    }
                    else if (it != checkedRanges_->get().end())
                    {
                        checkedRanges_->get().erase(it);
                    }
                    else
                    {
                        return false;
                    }

                    emit dataChanged(index, index);
                    return true;
                }
            }

            break;
        case (1):
            if (getRange(index).minimum() != floatValue)
            {
                getRange(index).setMinimum(floatValue);

                emit dataChanged(index, index);
                return true;
            }
            else
            {
                return false;
            }
            break;

        case (2):
            if (getRange(index).maximum() != floatValue)
            {
                getRange(index).setMaximum(floatValue);

                emit dataChanged(index, index);
                return true;
            }
            else
            {
                return false;
            }
            break;

        default:
            return false;
    }

    return false;
}

Qt::ItemFlags RangeModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

QVariant RangeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
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