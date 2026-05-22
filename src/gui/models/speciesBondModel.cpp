// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/speciesBondModel.h"

SpeciesBondModel::SpeciesBondModel() : bonds_(nullptr) {}

void SpeciesBondModel::reset()
{
    beginResetModel();
    endResetModel();
}

void SpeciesBondModel::setBonds(std::vector<SpeciesBond> &bonds)
{
    beginResetModel();
    bonds_ = &bonds;
    endResetModel();
}

int SpeciesBondModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!bonds_)
        return 0;
    return bonds_->size();
}

int SpeciesBondModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nDataTypes;
}

QVariant SpeciesBondModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    if (!bonds_)
        return {};

    auto &bond = bonds_->at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column())
        {
            case (DataType::IndexI):
            case (DataType::IndexJ):
                return bond.index(index.column()) + 1;
            case (DataType::Form):
                return bond.commonTerm()
                           ? QString::fromStdString("@" + std::string(bond.commonTerm()->name()))
                           : QString::fromStdString(std::string(BondFunctions::forms().keyword(bond.interactionForm())));
            case (DataType::Parameters):
                return bond.commonTerm()
                           ? QString::fromStdString(bond.commonTerm()->interactionPotential().parametersAsString())
                           : QString::fromStdString(bond.interactionPotential().parametersAsString());
            default:
                return {};
        }

    return {};
}

QVariant SpeciesBondModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    switch (section)
    {
        case (DataType::IndexI):
            return "i";
        case (DataType::IndexJ):
            return "J";
        case (DataType::Form):
            return "Form";
        case (DataType::Parameters):
            return "Parameters";
        default:
            return {};
    }
}

Qt::ItemFlags SpeciesBondModel::flags(const QModelIndex &index) const
{
    if (index.column() <= DataType::IndexJ)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > DataType::Form && bonds_->at(index.row()).commonTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesBondModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!bonds_)
        return false;
    auto &bond = bonds_->at(index.row());
    switch (index.column())
    {
        case (DataType::IndexI):
        case (DataType::IndexJ):
            return false;
        case (DataType::Form):
            try
            {
                auto bf = BondFunctions::forms().enumeration(value.toString().toStdString());
                bond.detachFromCommonTerm();
                bond.setInteractionForm(bf);
            }
            catch (std::runtime_error &e)
            {
                return false;
            }
            break;
        case (DataType::Parameters):
            if (!bond.setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }
    Q_EMIT dataChanged(index, index);
    return true;
}
