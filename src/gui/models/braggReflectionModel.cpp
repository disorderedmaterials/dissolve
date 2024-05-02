// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/braggReflectionModel.h"

// Set displayed reflections data
void BraggReflectionModel::setReflections(OptionalReferenceWrapper<const std::vector<BraggReflection>> optReflections)
{
    beginResetModel();
    reflections_ = optReflections;
    endResetModel();
}

// Set intensity column headers
void BraggReflectionModel::setIntensityHeaders(std::vector<std::string> headers)
{
    beginResetModel();
    intensityHeaders_ = std::move(headers);
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int BraggReflectionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return reflections_ ? reflections_->get().size() : 0;
}

int BraggReflectionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return reflections_ && reflections_->get().size() > 0 ? reflections_->get()[0].intensities().size() + 2 : 0;
}

QVariant BraggReflectionModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    auto &reflections = reflections_->get();

    switch (index.column())
    {
        case 0:
            return QString::number(reflections[index.row()].q());
        case 1:
            return QString("[%1 %2 %3]")
                .arg(reflections[index.row()].hkl().x)
                .arg(reflections[index.row()].hkl().y)
                .arg(reflections[index.row()].hkl().z);
        case 2:
            return QString::number(reflections[index.row()].nKVectors());
        default:
            if (index.column() - 3 < reflections[index.row()].intensities().size())
                return QString::number(reflections[index.row()].intensities()[index.column() - 3]);
            else
                return {};
    }
}

QVariant BraggReflectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Q";
            case 1:
                return "[h k l]";
            case 2:
                return "Multiplicity";
            default:
                if (section - 3 < intensityHeaders_.size())
                    return QVariant(QString::fromStdString(intensityHeaders_[section - 3]));
                else
                    return {};
        }

    return {};
}
