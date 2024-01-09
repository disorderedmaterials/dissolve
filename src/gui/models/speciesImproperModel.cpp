// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesImproperModel.h"
#include "classes/coreData.h"

SpeciesImproperModel::SpeciesImproperModel(std::vector<SpeciesImproper> &impropers, const CoreData &coreData)
    : impropers_(impropers), coreData_(coreData)
{
}

void SpeciesImproperModel::reset()
{
    beginResetModel();
    endResetModel();
}

int SpeciesImproperModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return impropers_.size();
}

int SpeciesImproperModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nDataTypes;
}

QVariant SpeciesImproperModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &improper = impropers_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&improper);

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column())
        {
            case (DataType::IndexI):
            case (DataType::IndexJ):
            case (DataType::IndexK):
            case (DataType::IndexL):
                return improper.index(index.column()) + 1;
            case (DataType::Form):
                return improper.masterTerm()
                           ? QString::fromStdString("@" + std::string(improper.masterTerm()->name()))
                           : QString::fromStdString(TorsionFunctions::forms().keyword(improper.interactionForm()));
            case (DataType::Parameters):
                return improper.masterTerm()
                           ? QString::fromStdString(improper.masterTerm()->interactionPotential().parametersAsString())
                           : QString::fromStdString(improper.interactionPotential().parametersAsString());
            default:
                return {};
        }

    return {};
}

QVariant SpeciesImproperModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    switch (section)
    {
        case (DataType::IndexI):
            return "I";
        case (DataType::IndexJ):
            return "J";
        case (DataType::IndexK):
            return "K";
        case (DataType::IndexL):
            return "L";
        case (DataType::Form):
            return "Form";
        case (DataType::Parameters):
            return "Parameters";
        default:
            return {};
    }
}

Qt::ItemFlags SpeciesImproperModel::flags(const QModelIndex &index) const
{
    if (index.column() <= DataType::IndexL)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > DataType::Form && impropers_[index.row()].masterTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesImproperModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &improper = impropers_[index.row()];
    switch (index.column())
    {
        case (DataType::IndexI):
        case (DataType::IndexJ):
        case (DataType::IndexK):
        case (DataType::IndexL):
            return false;
        case (DataType::Form):
            if (value.toString().at(0) == '@')
            {
                auto master = coreData_.getMasterImproper(value.toString().toStdString());
                if (master)
                    improper.setMasterTerm(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    auto tf = TorsionFunctions::forms().enumeration(value.toString().toStdString());
                    improper.detachFromMasterTerm();
                    improper.setInteractionForm(tf);
                }
                catch (std::runtime_error &e)
                {
                    return false;
                }
            }
            break;
        case (DataType::Parameters):
            if (!improper.setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }
    Q_EMIT dataChanged(index, index);
    return true;
}
