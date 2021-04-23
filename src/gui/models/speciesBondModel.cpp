#include "gui/models/speciesBondModel.h"

SpeciesBondModel::SpeciesBondModel(std::vector<SpeciesBond> &bonds) : bonds_(bonds) {}

int SpeciesBondModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return bonds_.size();
}

int SpeciesBondModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QVariant SpeciesBondModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    if (role != Qt::DisplayRole)
        return QVariant();

    auto &bond = bonds_[index.row()];

    switch (index.column())
    {
        case 0:
        case 1:
            return bond.index(index.column()) + 1;
        case 2:
            return bond.masterParameters() ? ("@" + std::string(bond.masterParameters()->name())).c_str()
                                           : std::string(SpeciesBond::bondFunctions().keywordFromInt(bond.form())).c_str();
        case 3:
        case 4:
        case 5:
        case 6:
            return bond.parameter(index.column() - 3);
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
            return "Parameter 1";
        case 4:
            return "Parameter 2";
        case 5:
            return "Parameter 3";
        case 6:
            return "Parameter 4";
        default:
            return QVariant();
    }
}
