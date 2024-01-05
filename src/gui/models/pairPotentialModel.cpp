// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/pairPotentialModel.h"
#include "base/sysFunc.h"
#include "templates/algorithms.h"

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
    auto *pp = rawData(index);
    if (!pp)
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            // Name
            case (0):
                return QString::fromStdString(std::string(pp->atomTypeNameI()));
            // Element
            case (1):
                return QString::fromStdString(std::string(pp->atomTypeNameJ()));
            // Form
            case (2):
                return QString::fromStdString(ShortRangeFunctions::forms().keyword(pp->interactionPotential().form()));
            // Charges
            case (3):
                return pp->includeAtomTypeCharges() ? QString::number(pp->chargeI()) : QString();
            case (4):
                return pp->includeAtomTypeCharges() ? QString::number(pp->chargeJ()) : QString();
            // Short Range Parameters
            case (5):
                return QString::fromStdString(pp->interactionPotential().parametersAsString());
            default:
                return {};
        }
    }
    else if (role == Qt::UserRole)
        return QVariant::fromValue(pp);

    return {};
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
            if (!pair->interactionPotential().parseParameters(value.toString().toStdString()))
                return false;
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
        return {};

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
                return {};
        }

    return {};
}

// Set source AtomType data
void PairPotentialModel::reset()
{
    beginResetModel();
    endResetModel();
}
