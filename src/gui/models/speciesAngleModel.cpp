// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesAngleModel.h"
#include "classes/coreData.h"

SpeciesAngleModel::SpeciesAngleModel(std::vector<SpeciesAngle> &angles, const CoreData &coreData)
    : angles_(angles), coreData_(coreData)
{
}

void SpeciesAngleModel::reset()
{
    beginResetModel();
    endResetModel();
}

int SpeciesAngleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return angles_.size();
}

int SpeciesAngleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nDataTypes;
}

QVariant SpeciesAngleModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &angle = angles_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&angle);

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column())
        {
            case (DataType::IndexI):
            case (DataType::IndexJ):
            case (DataType::IndexK):
                return angle.index(index.column()) + 1;
            case (DataType::Form):
                return angle.masterTerm() ? QString::fromStdString("@" + std::string(angle.masterTerm()->name()))
                                          : QString::fromStdString(AngleFunctions::forms().keyword(angle.interactionForm()));
            case (DataType::Parameters):
                return angle.masterTerm()
                           ? QString::fromStdString(angle.masterTerm()->interactionPotential().parametersAsString())
                           : QString::fromStdString(angle.interactionPotential().parametersAsString());
            default:
                return {};
        }

    return {};
}

QVariant SpeciesAngleModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case (DataType::Form):
            return "Form";
        case (DataType::Parameters):
            return "Parameters";
        default:
            return {};
    }
}

Qt::ItemFlags SpeciesAngleModel::flags(const QModelIndex &index) const
{
    if (index.column() <= DataType::IndexK)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > DataType::Form && angles_[index.row()].masterTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesAngleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &angle = angles_[index.row()];
    switch (index.column())
    {
        case (DataType::IndexI):
        case (DataType::IndexJ):
        case (DataType::IndexK):
            return false;
        case (DataType::Form):
            if (value.toString().at(0) == '@')
            {
                auto master = coreData_.getMasterAngle(value.toString().toStdString());
                if (master)
                    angle.setMasterTerm(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    auto af = AngleFunctions::forms().enumeration(value.toString().toStdString());
                    angle.detachFromMasterTerm();
                    angle.setInteractionForm(af);
                }
                catch (std::runtime_error &e)
                {
                    return false;
                }
            }
            break;
        case (DataType::Parameters):
            if (!angle.setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
