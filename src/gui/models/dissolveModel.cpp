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
    switch(index.row())
    {
        case 0:
            return QVariant::fromValue(dissolve_->get().coreData().species()[index.column()].get());
        case 1:
            return QVariant::fromValue(dissolve_->get().coreData().configurations()[index.column()].get());
        default:
            return {};
    }
}

const QVariant DissolveModel::rawData(const QModelIndex index) const
{
    switch(index.row())
    {
        case 0:
            Messenger::print("Get Species {}/{}", index.column(), dissolve_->get().coreData().nSpecies());
            return QVariant::fromValue(dissolve_->get().coreData().species()[index.column()].get());
        case 1:
            Messenger::print("Get Configuration {}/{}", index.column(), dissolve_->get().coreData().nConfigurations());
            return QVariant::fromValue(dissolve_->get().coreData().configurations()[index.column()].get());
        default:
            return {};
    }
}

QModelIndex DissolveModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int DissolveModel::columnCount(const QModelIndex &parent) const
{
    return std::max(dissolve_->get().coreData().nConfigurations(), dissolve_->get().coreData().nSpecies());
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
            case (0):
                return QString::fromStdString(std::string(d.value<Species*>()->name()));
            case (1):
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
    return 2;
}

QModelIndex DissolveModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}