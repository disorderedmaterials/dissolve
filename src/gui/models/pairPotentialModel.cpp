// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/pairPotentialModel.h"
#include "base/sysFunc.h"
#include "templates/algorithms.h"

PairPotentialModel::PairPotentialModel(const std::vector<PairPotential::Definition> &data) : data_(data) {}

/*
 * QAbstractItemModel overrides
 */

int PairPotentialModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return data_.size();
}

int PairPotentialModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Columns::nDataColumns;
}

// Set whether the data is editable or not
void PairPotentialModel::setEditable(bool b)
{
    if (editable_ == b)
        return;

    editable_ = b;

    reset();
}

const PairPotential *PairPotentialModel::rawData(const QModelIndex index) const
{
    return std::get<2>(data_[index.row()]).get();
}

PairPotential *PairPotentialModel::rawData(const QModelIndex index) { return std::get<2>(data_[index.row()]).get(); }

QVariant PairPotentialModel::data(const QModelIndex &index, int role) const
{
    auto *pp = rawData(index);
    if (!pp)
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case (Columns::NameIColumn):
                return QString::fromStdString(std::string(pp->nameI()));
            case (Columns::NameJColumn):
                return QString::fromStdString(std::string(pp->nameJ()));
            case (Columns::ChargeIColumn):
                return pp->includeAtomTypeCharges() ? QString::number(pp->chargeI()) : QString();
            case (Columns::ChargeJColumn):
                return pp->includeAtomTypeCharges() ? QString::number(pp->chargeJ()) : QString();
            case (Columns::ShortRangeFormColumn):
                return QString::fromStdString(Functions1D::forms().keyword(pp->interactionPotential().form()));
            case (Columns::ShortRangeParametersColumn):
                return QString::fromStdString(pp->interactionPotential().parametersAsString());
            default:
                return {};
        }
    }
    else if (role == Qt::UserRole)
        return QVariant::fromValue(pp);

    return {};
}

bool PairPotentialModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || !editable_)
        return false;

    auto *pair = rawData(index);

    switch (index.column())
    {
        // Uneditable Columns
        case (Columns::NameIColumn):
        case (Columns::NameJColumn):
        case (Columns::ChargeIColumn):
        case (Columns::ChargeJColumn):
            return false;
        case (Columns::ShortRangeFormColumn):
            pair->setInteractionPotentialForm(Functions1D::forms().enumeration(value.toString().toStdString()));
            break;
        case (Columns::ShortRangeParametersColumn):
            if (!pair->setInteractionPotential(pair->interactionPotential().form(), value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    Q_EMIT dataChanged(index, index);

    return true;
}

Qt::ItemFlags PairPotentialModel::flags(const QModelIndex &index) const
{
    return editable_ &&
                   (index.column() == Columns::ShortRangeFormColumn || index.column() == Columns::ShortRangeParametersColumn)
               ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable
               : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant PairPotentialModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (Columns::NameIColumn):
                return "Type I";
            case (Columns::NameJColumn):
                return "Type J";
            case (Columns::ChargeIColumn):
                return "Charge I";
            case (Columns::ChargeJColumn):
                return "Charge J";
            case (Columns::ShortRangeFormColumn):
                return "Form";
            case (Columns::ShortRangeParametersColumn):
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
