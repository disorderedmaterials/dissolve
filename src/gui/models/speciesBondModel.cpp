// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesBondModel.h"
#include "classes/coreData.h"

SpeciesBondModel::SpeciesBondModel(std::vector<SpeciesBond> &bonds, const CoreData &coreData)
    : bonds_(bonds), coreData_(coreData)
{
}

void SpeciesBondModel::reset()
{
    beginResetModel();
    endResetModel();
}

int SpeciesBondModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return bonds_.size();
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

    auto &bond = bonds_[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column())
        {
            case (DataType::IndexI):
            case (DataType::IndexJ):
                return bond.index(index.column()) + 1;
            case (DataType::Form):
                return bond.masterTerm()
                           ? QString::fromStdString("@" + std::string(bond.masterTerm()->name()))
                           : QString::fromStdString(std::string(BondFunctions::forms().keyword(bond.interactionForm())));
            case (DataType::Parameters):
                return bond.masterTerm()
                           ? QString::fromStdString(bond.masterTerm()->interactionPotential().parametersAsString())
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
    if (index.column() > DataType::Form && bonds_[index.row()].masterTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesBondModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &bond = bonds_[index.row()];
    switch (index.column())
    {
        case (DataType::IndexI):
        case (DataType::IndexJ):
            return false;
        case (DataType::Form):
            if (value.toString().at(0) == '@')
            {
                auto master = coreData_.getMasterBond(value.toString().toStdString());
                if (master)
                    bond.setMasterTerm(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    auto bf = BondFunctions::forms().enumeration(value.toString().toStdString());
                    bond.detachFromMasterTerm();
                    bond.setInteractionForm(bf);
                }
                catch (std::runtime_error &e)
                {
                    return false;
                }
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
