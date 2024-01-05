// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesTorsionModel.h"
#include "classes/coreData.h"

SpeciesTorsionModel::SpeciesTorsionModel(std::vector<SpeciesTorsion> &torsions, const CoreData &coreData)
    : torsions_(torsions), coreData_(coreData)
{
}

void SpeciesTorsionModel::reset()
{
    beginResetModel();
    endResetModel();
}

int SpeciesTorsionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return torsions_.size();
}

int SpeciesTorsionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nDataTypes;
}

QVariant SpeciesTorsionModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &torsion = torsions_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&torsion);

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column())
        {
            case (DataType::IndexI):
            case (DataType::IndexJ):
            case (DataType::IndexK):
            case (DataType::IndexL):
                return torsion.index(index.column()) + 1;
            case (DataType::Form):
                return torsion.masterTerm()
                           ? QString::fromStdString("@" + std::string(torsion.masterTerm()->name()))
                           : QString::fromStdString(TorsionFunctions::forms().keyword(torsion.interactionForm()));
            case (DataType::Parameters):
                return torsion.masterTerm()
                           ? QString::fromStdString(torsion.masterTerm()->interactionPotential().parametersAsString())
                           : QString::fromStdString(torsion.interactionPotential().parametersAsString());
            case (DataType::Electrostatic14Scale):
                return torsion.masterTerm() ? QString::number(torsion.masterTerm()->electrostatic14Scaling())
                                            : QString::number(torsion.electrostatic14Scaling());
            case (DataType::VanDerWaals14Scale):
                return torsion.masterTerm() ? QString::number(torsion.masterTerm()->vanDerWaals14Scaling())
                                            : QString::number(torsion.vanDerWaals14Scaling());
            default:
                return {};
        }

    return {};
}

QVariant SpeciesTorsionModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case (DataType::Electrostatic14Scale):
            return "Elec 1-4";
        case (DataType::VanDerWaals14Scale):
            return "vdW 1-4";
        default:
            return {};
    }
}

Qt::ItemFlags SpeciesTorsionModel::flags(const QModelIndex &index) const
{
    if (index.column() <= DataType::IndexL)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > DataType::Form && torsions_[index.row()].masterTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesTorsionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &torsion = torsions_[index.row()];
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
                auto master = coreData_.getMasterTorsion(value.toString().toStdString());
                if (master)
                    torsion.setMasterTerm(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    auto tf = TorsionFunctions::forms().enumeration(value.toString().toStdString());
                    torsion.detachFromMasterTerm();
                    torsion.setInteractionForm(tf);
                }
                catch (std::runtime_error &e)
                {
                    return false;
                }
            }
            break;
        case (DataType::Parameters):
            if (!torsion.setInteractionParameters(value.toString().toStdString()))
                return false;
        case (DataType::Electrostatic14Scale):
            if (!torsion.setElectrostatic14Scaling(value.toDouble()))
                return false;
            break;
        case (DataType::VanDerWaals14Scale):
            if (!torsion.setVanDerWaals14Scaling(value.toDouble()))
                return false;
            break;
        default:
            return false;
    }

    emit dataChanged(index, index);

    return true;
}
