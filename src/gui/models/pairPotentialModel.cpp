// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/pairPotentialModel.h"
#include "base/sysfunc.h"
#include "templates/algorithms.h"

Q_DECLARE_METATYPE(PairPotential *);
Q_DECLARE_METATYPE(const PairPotential *);

PairPotentialModel::PairPotentialModel(const std::vector<std::unique_ptr<PairPotential>> &pairs) : pairs_(pairs) {}

/*
 * QAbstractItemModel overrides
 */

int PairPotentialModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return pairs_.size();
}

int PairPotentialModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

const PairPotential *PairPotentialModel::rawData(const QModelIndex index) const { return pairs_[index.row()].get(); }

PairPotential *PairPotentialModel::rawData(const QModelIndex index) { return pairs_[index.row()].get(); }

QVariant PairPotentialModel::data(const QModelIndex &index, int role) const
{
    // return QString::number(pairs_.size());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            // Name
            case (0):
                return QString::fromStdString(std::string(rawData(index)->atomTypeNameI()));
            // Element
            case (1):
                return QString::fromStdString(std::string(rawData(index)->atomTypeNameJ()));
            // Charge
            case (2):
                return QString::fromStdString(
                    std::string(Forcefield::shortRangeTypes().keyword(rawData(index)->shortRangeType())));
            // Short Range Parameters
            case (3):
                return QString::number(rawData(index)->chargeI());
            case (4):
                return QString::number(rawData(index)->chargeJ());
            case (5):
                return QString::fromStdString(joinStrings(rawData(index)->parameters()));
            default:
                return QVariant();
        }
    }

    return QVariant();
}

// This function is extraneous right now, since none of the cells are
// editable, but they're included for completeness.
bool PairPotentialModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    auto *pair = rawData(index);
    std::vector<double> values;

    switch (index.column())
    {
        // Name
        case (0):
        case (1):
        case (2):
            return false;
        case (3):
            pair->setChargeI(value.toDouble());
            break;
        case (4):
            pair->setChargeJ(value.toDouble());
            break;
        // Short Range Parameters
        case (5):
            values = DissolveSys::splitStringToDoubles(value.toString().toStdString());
            if (values.size() != pair->parameters().size())
                return false;
            {
                int idx = 0;
                for (auto v : values)
                    pair->setParameter(idx++, v);
            }
            break;
        default:
            return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags PairPotentialModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

QVariant PairPotentialModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (0):
                return "Type I";
            case (1):
                return "Type J";
            case (2):
                return "Form";
            case (3):
                return "Charge I";
            case (4):
                return "Charge J";
            case (5):
                return "Parameters";
            default:
                return QVariant();
        }

    return QVariant();
}

// Set source AtomType data
void PairPotentialModel::reset()
{
    beginResetModel();
    endResetModel();
}
