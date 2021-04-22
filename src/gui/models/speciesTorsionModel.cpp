#include "gui/models/speciesTorsionModel.h"
#include <algorithm>

SpeciesTorsionModel::SpeciesTorsionModel() {}

int SpeciesTorsionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 0;
}

int SpeciesTorsionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 9;
}

QVariant SpeciesTorsionModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
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
