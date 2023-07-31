// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/rangeModel.h"

// set range data
void RangeModel::setData(OptionalReferenceWrapper<std::vector<Range>> &ranges)
{
    if (ranges)
    {
        beginResetModel();
        ranges_ = ranges;
        endResetModel();
    }
}

Range RangeModel::getRange(const QModelIndex &index) const { return ranges_[index.row()]; }

/*
 * QAbstractItemModel overrides
 */

int RangeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (ranges_ ? ranges_.size() : 0);
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
                    return QString::number(index.row());
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
            if (index.column != 0)
            {
                return {};
            }

            // See if the selected cell's row index is stored in the checkedRanges_ vector
            return std::find(checkedRanges_.begin(), checkedRanges_.end(), index.row()) == checkedRanges_.end() ? Qt::Unchecked
                                                                                                                : Qt::Checked;

        default:
            return {};
    }
}

bool RangeModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
{
    if (!ranges_)
    {
        return false;
    }

    auto range = getRange(index);

    switch (index.column())
    {
        case (0):
            if (role == Qt::CheckStateRole)
            {
                // See if the selected cell's row index is stored in the checkedRanges_ vector
                auto it = find(checkedRanges_.begin(), checkedRanges_.end(), index.row());

                if (value.value<Qt::CheckState>() == Qt::Checked)
                {
                    // If checked cell is not stored in checkedRanges_, add it
                    if (it == checkedRanges_.end())
                    {
                        checkedRanges.push_back(index.row());
                    }
                    else
                    {
                        return false;
                    }
                }

                else
                {
                    // If unchecked cell is stored in checkedRanges_, remove it
                    if (it != checkedRanges_.end())
                    {
                        checkedRanges_.erase(it);
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
            if (getRange(index).minimum() != value)
            {
                getRange(index).setMinimum(value);

                emit dataChanged(index, index);
                return true;
            }
            else
            {
                return false;
            }
            break;

        case (2):
            if (getRange(index).maximum() != value)
            {
                getRange(index).setMaximum(value);

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
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

QVariant RangeModel::headerData(int section, Qt::Orientation oriantation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Enabled";
            case 1:
                return "Minimum";
            case 2:
                return "Maximum";
            default:
                return {};
        }

    return {};
}