#include "gui/models/dissolveModel.h"

DissolveModel::DissolveModel(Dissolve &dissolve) : dissolve_(dissolve)
{}

void DissolveModel::setDissolve(Dissolve &dissolve)
{
    beginResetModel();
    dissolve_ = dissolve;
    endResetModel();
}

QVariant DissolveModel::rawData(const QModelIndex index)
{
    if (!index.isValid())
        return {};
    if (!dissolve_)
        return {};
    auto& species = dissolve_->get().coreData().species();
    auto& configurations = dissolve_->get().coreData().configurations();
    switch(index.row())
    {
        case (0):
            return {};
        case (1):
            if (index.column() < species.size())
                return QVariant::fromValue(dissolve_->get().coreData().species()[index.column()].get());
            break;
        case (2):
            if (index.column() < configurations.size())
                return QVariant::fromValue(dissolve_->get().coreData().configurations()[index.column()].get());
            break;
        default:
            return {};
    }
    return {};
}

const QVariant DissolveModel::rawData(const QModelIndex index) const
{
    if (!index.isValid())
        return {};
    if (!dissolve_)
        return {};
    auto& species = dissolve_->get().coreData().species();
    auto& configurations = dissolve_->get().coreData().configurations();
    switch(index.row())
    {
        case (0):
            switch (index.column())
            {
                case (0):
                    return QVariant::fromValue(dissolve_->get().coreData().atomTypes());
                case (1):
                    return QVariant::fromValue(&dissolve_->get().coreData());
                default:
                    return {};
            }
        case (1):
            if (index.column() < species.size())
                return QVariant::fromValue(dissolve_->get().coreData().species()[index.column()].get());
            break;
        case (2):
            if (index.column() < configurations.size())
                return QVariant::fromValue(dissolve_->get().coreData().configurations()[index.column()].get());
            break;
        default:
            return {};
    }
    return {};
}

QModelIndex DissolveModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int DissolveModel::columnCount(const QModelIndex &parent) const
{
    auto max = std::max(dissolve_->get().coreData().nConfigurations(), dissolve_->get().coreData().nSpecies());
    return std::max(max, 2);
}

QVariant DissolveModel::data(const QModelIndex &index, int role) const
{
    auto d = rawData(index);
    if (d.isNull())
         return {};
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.row())
        {
            case 0:
                switch (index.column())
                {
                    case (0):
                        return "Atom Types";
                    case (1):
                        return "Master Terms";
                    default:
                        return {};
                }
            case (1):
                return QString::fromStdString(std::string(d.value<Species*>()->name()));
            case (2):
                return QString::fromStdString(std::string(d.value<Configuration*>()->name()));
            default:
                return {};
        }
    }
    else if (role == Qt::UserRole)
        return d;
    return {};
}

int DissolveModel::rowCount(const QModelIndex &parent) const
{
    return 3;
}

QModelIndex DissolveModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

void DissolveModel::reset()
{
    beginResetModel();
    endResetModel();
}