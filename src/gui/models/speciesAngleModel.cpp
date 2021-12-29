// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/speciesAngleModel.h"
#include "classes/coredata.h"

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
    return 5;
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
            case 0:
            case 1:
            case 2:
                return angle.index(index.column()) + 1;
            case 3:
                return angle.masterTerm() ? QString::fromStdString("@" + std::string(angle.masterTerm()->name()))
                                          : QString::fromStdString(AngleFunctions::forms().keyword(angle.interactionForm()));
            case 4:
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
        case 0:
            return "I";
        case 1:
            return "J";
        case 2:
            return "K";
        case 3:
            return "Form";
        case 4:
            return "Parameters";
        default:
            return {};
    }
}

Qt::ItemFlags SpeciesAngleModel::flags(const QModelIndex &index) const
{
    if (index.column() < 3)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > 3 && angles_[index.row()].masterTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesAngleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &angle = angles_[index.row()];
    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
            return false;
        case 3:
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
        case 4:
            if (!angle.setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
