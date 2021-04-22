#include "gui/models/speciesAngleModel.h"

SpeciesAngleModel::SpeciesAngleModel() {}

int SpeciesAngleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 0;
}

int SpeciesAngleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

QVariant SpeciesAngleModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        switch (index.column())
        {
            case 0:
                return "AtomI";
            case 1:
                return "AtomJ";
            case 2:
                return "AtomK";
            case 3:
                return "Angle";
            case 4:
                return "k";
            default:
                return QVariant();
        }

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
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
