#include "gui/models/speciesTorsionModel.h"
#include "classes/masterintra.h"
#include "gui/models/speciesModelUtils.h"
#include <algorithm>

SpeciesTorsionModel::SpeciesTorsionModel(std::vector<SpeciesTorsion> &torsions, Dissolve &dissolve)
    : torsions_(torsions), dissolve_(dissolve)
{
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
        headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &item = torsions_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&item);

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
        case 3:
            return item.index(index.column()) + 1;
        case 4:
            return item.masterParameters()
                       ? QString::fromStdString("@" + std::string(item.masterParameters()->name()))
                       : QString::fromStdString(SpeciesTorsion::torsionFunctions().keywordFromInt(item.form()));
        case 5:
            return joinParameters(item);
        default:
            return QVariant();
    }
}

QVariant SpeciesTorsionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
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
            return QVariant();
    }
}

Qt::ItemFlags SpeciesTorsionModel::flags(const QModelIndex &index) const
{
    if (index.column() < 4)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > 4 && torsions_[index.row()].masterParameters())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesTorsionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &item = torsions_[index.row()];
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
                auto master = dissolve_.coreData().getMasterTorsion(value.toString().toStdString());
                if (master)
                    item.setMasterParameters(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    SpeciesTorsion::TorsionFunction bf =
                        SpeciesTorsion::torsionFunctions().enumeration(value.toString().toStdString());
                    item.detachFromMasterIntra();
                    item.setForm(bf);
                    return true;
                }
                catch (std::runtime_error e)
                {
                    return false;
                }
            }
            break;
        case 5:
            if (!splitParameters(value.toString(), item))
                return false;
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
