#include "gui/models/speciesAtomModel.h"

SpeciesAtomModel::SpeciesAtomModel() {}

int SpeciesAtomModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 0;
}

int SpeciesAtomModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

QVariant SpeciesAtomModel::data(const QModelIndex &index, int role) const
{
    int type;

    if (role == Qt::ToolTipRole)
        switch (index.column())
        {
            case 0:
                return "E";
            case 1:
                return "AtomType";
            case 2:
                return "X";
            case 3:
                return "Y";
            case 4:
                return "Z";
            case 5:
                return "Charge";
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

QVariant SpeciesAtomModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
    case 0:
      return "E";
    case 1:
      return "AtomType";
    case 2:
      return "X";
    case 3:
      return "Y";
    case 4:
      return "Z";
    case 5:
      return "Charge";
    default:
      return QVariant();
    }
}

void SpeciesAtomModel::clear()
{
    beginResetModel();
    endResetModel();
}
