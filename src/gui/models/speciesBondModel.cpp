#include "gui/models/speciesBondModel.h"

SpeciesBondModel::SpeciesBondModel() {}

int SpeciesBondModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 0;
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
    switch (index.column())
    {
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
