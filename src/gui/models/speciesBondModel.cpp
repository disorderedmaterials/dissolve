#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesModelUtils.h"

SpeciesBondModel::SpeciesBondModel(std::vector<SpeciesBond> &bonds, Dissolve &dissolve) : bonds_(bonds), dissolve_(dissolve) {}

int SpeciesBondModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return bonds_.size();
}

int SpeciesBondModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant SpeciesBondModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    auto &bond = bonds_[index.row()];

    switch (index.column())
    {
        case 0:
        case 1:
            return bond.index(index.column()) + 1;
        case 2:
            return bond.masterParameters()
                       ? QString::fromStdString("@" + std::string(bond.masterParameters()->name()))
                       : QString::fromStdString(std::string(SpeciesBond::bondFunctions().keywordFromInt(bond.form())));
        case 3:
            return joinParameters(bond);
        default:
            return QVariant();
    }
}

QVariant SpeciesBondModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "i";
        case 1:
            return "J";
        case 2:
            return "Form";
        case 3:
            return "Parameters";
        default:
            return QVariant();
    }
}

Qt::ItemFlags SpeciesBondModel::flags(const QModelIndex &index) const
{
    if (index.column() < 2)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > 2 && bonds_[index.row()].masterParameters())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesBondModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &item = bonds_[index.row()];
    switch (index.column())
    {
        case 0:
        case 1:
            return false;
        case 2:
            if (value.toString().at(0) == '@')
            {
                auto master = dissolve_.coreData().getMasterBond(value.toString().toStdString());
                if (master)
                    item.setMasterParameters(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    SpeciesBond::BondFunction bf = SpeciesBond::bondFunctions().enumeration(value.toString().toStdString());
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
        case 3:
            if (!splitParameters(value.toString(), item))
                return false;
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
