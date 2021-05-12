#include "gui/models/speciesImproperModel.h"
#include "classes/masterintra.h"

SpeciesImproperModel::SpeciesImproperModel(std::vector<SpeciesImproper> &impropers) : impropers_(impropers) {}

int SpeciesImproperModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return impropers_.size();
}

int SpeciesImproperModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 9;
}

QVariant SpeciesImproperModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &item = impropers_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&item);

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
        case 3:
            return item.index(index.column()) + 1;
        case 4:
            return item.masterParameters() ? ("@" + std::string(item.masterParameters()->name())).c_str()
                                           : SpeciesTorsion::torsionFunctions().keywordFromInt(item.form()).c_str();
        case 5:
        case 6:
        case 7:
        case 8:
            return item.parameter(index.column() - 5);
        default:
            return QVariant();
    }
}

QVariant SpeciesImproperModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return "Parameter 1";
        case 6:
            return "Parameter 2";
        case 7:
            return "Parameter 3";
        case 8:
            return "Parameter 4";

        default:
            return QVariant();
    }
}
