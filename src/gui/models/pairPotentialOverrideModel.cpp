// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/pairPotentialOverrideModel.h"
#include "base/sysFunc.h"

PairPotentialOverrideModel::PairPotentialOverrideModel(std::vector<std::unique_ptr<PairPotentialOverride>> &data) : data_(data)
{
}

/*
 * QAbstractItemModel overrides
 */

int PairPotentialOverrideModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return data_.size();
}

int PairPotentialOverrideModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnData::nColumnData;
}

const PairPotentialOverride *PairPotentialOverrideModel::rawData(const QModelIndex index) const
{
    return data_[index.row()].get();
}

PairPotentialOverride *PairPotentialOverrideModel::rawData(const QModelIndex index) { return data_[index.row()].get(); }

QVariant PairPotentialOverrideModel::data(const QModelIndex &index, int role) const
{
    auto *ppOverride = rawData(index);
    if (!ppOverride)
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case (ColumnData::MatchI):
                return QString::fromStdString(std::string(ppOverride->matchI()));
            case (ColumnData::MatchJ):
                return QString::fromStdString(std::string(ppOverride->matchJ()));
            case (ColumnData::OverrideType):
                return QString::fromStdString(PairPotentialOverride::pairPotentialOverrideTypes().keyword(ppOverride->type()));
            case (ColumnData::ShortRangeForm):
                return QString::fromStdString(ShortRangeFunctions::forms().keyword(ppOverride->interactionPotential().form()));
            case (ColumnData::ShortRangeParameters):
                return QString::fromStdString(ppOverride->interactionPotential().parametersAsString());
            default:
                return {};
        }
    }
    else if (role == Qt::UserRole)
        return QVariant::fromValue(ppOverride);

    return {};
}

bool PairPotentialOverrideModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    auto *ppOverride = rawData(index);

    switch (index.column())
    {
        case (ColumnData::MatchI):
            ppOverride->setMatchI(value.toString().toStdString());
            break;
        case (ColumnData::MatchJ):
            ppOverride->setMatchJ(value.toString().toStdString());
            break;
        case (ColumnData::OverrideType):
            ppOverride->setType(
                PairPotentialOverride::pairPotentialOverrideTypes().enumeration(value.toString().toStdString()));
            break;
        case (ColumnData::ShortRangeForm):
            ppOverride->interactionPotential().setForm(
                ShortRangeFunctions::forms().enumeration(value.toString().toStdString()));
            break;
        case (ColumnData::ShortRangeParameters):
            if (!ppOverride->interactionPotential().parseParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    Q_EMIT dataChanged(index, index);

    return true;
}

Qt::ItemFlags PairPotentialOverrideModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant PairPotentialOverrideModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section)
    {
        case (ColumnData::MatchI):
            return "Match I";
        case (ColumnData::MatchJ):
            return "Match J";
        case (ColumnData::OverrideType):
            return "Type";
        case (ColumnData::ShortRangeForm):
            return "Form";
        case (ColumnData::ShortRangeParameters):
            return "Parameters";
        default:
            return {};
    }
}

// Set source AtomType data
void PairPotentialOverrideModel::reset()
{
    beginResetModel();
    endResetModel();
}
