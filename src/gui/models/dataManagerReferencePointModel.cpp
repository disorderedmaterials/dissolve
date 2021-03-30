#include "gui/models/dataManagerModel.h"
#include "templates/variantpointer.h"

DataManagerReferencePointModel::DataManagerReferencePointModel(std::vector<ReferencePoint> &referencePoints)
    : referencePoints_(referencePoints)
{
}

int DataManagerReferencePointModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return referencePoints_.size();
}

int DataManagerReferencePointModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant DataManagerReferencePointModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto &item = referencePoints_[index.row()];

    switch (role)
    {
        case Qt::DisplayRole:
            if (index.column() == 0)
                return std::string(item.suffix()).c_str();
            return std::string(item.restartFile()).c_str();
            ;
        case Qt::UserRole:
            return VariantPointer<ReferencePoint>(&item);
        default:
            return QVariant();
    }
}

QVariant DataManagerReferencePointModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "Data Suffix";
        case 1:
            return "Source";
        default:
            return QVariant();
    }
}

void DataManagerReferencePointModel::update()
{
    beginResetModel();
    endResetModel();
}
