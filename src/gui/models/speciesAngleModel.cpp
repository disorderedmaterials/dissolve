#include "gui/models/speciesAngleModel.h"
#include "classes/masterintra.h"

SpeciesAngleModel::SpeciesAngleModel(std::vector<SpeciesAngle> &angles) : angles_(angles) {}

int SpeciesAngleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return angles_.size();
}

int SpeciesAngleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

QVariant SpeciesAngleModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &angle = angles_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&angle);

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
            return angle.index(index.column()) + 1;
        case 3:
            angle.masterParameters() ? ("@" + std::string(angle.masterParameters()->name())).c_str()
                                     : SpeciesAngle::angleFunctions().keywordFromInt(angle.form());
        case 4:
        case 5:
        case 6:
        case 7:
            return angle.parameter(index.column() - 4);
        default:
            return QVariant();
    }
}

QVariant SpeciesAngleModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return "Form";
        case 4:
            return "Parameter 1";
        case 5:
            return "Parameter 2";
        case 6:
            return "Parameter 3";
        case 7:
            return "Parameter 4";
        default:
            return QVariant();
    }
}
