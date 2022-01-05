// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/speciesTorsionModel.h"
#include "classes/coredata.h"

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
    return 6;
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
            case 0:
            case 1:
            case 2:
            case 3:
                return torsion.index(index.column()) + 1;
            case 4:
                return torsion.masterTerm() ? QString::fromStdString("@" + std::string(torsion.masterTerm()->name()))
                                            : QString::fromStdString(TorsionFunctions::forms().keyword(torsion.form()));
            case 5:
                return QString::fromStdString(torsion.parametersAsString());
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
        case 0:
            return "I";
        case 1:
            return "J";
        case 2:
            return "K";
        case 3:
            return "L";

        case 4:
            return "Form";

        case 5:
            return "Parameters";

        default:
            return {};
    }
}

Qt::ItemFlags SpeciesTorsionModel::flags(const QModelIndex &index) const
{
    if (index.column() < 4)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > 4 && torsions_[index.row()].masterTerm())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesTorsionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &torsion = torsions_[index.row()];
    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
        case 3:
            return false;
        case 4:
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
                    torsion.setForm(tf);
                }
                catch (std::runtime_error &e)
                {
                    return false;
                }
            }
            break;
        case 5:
            if (!torsion.setParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
