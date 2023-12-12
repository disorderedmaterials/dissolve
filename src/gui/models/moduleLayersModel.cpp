// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/moduleLayersModel.h"
#include "gui/models/moduleLayerModel.h"
#include "qabstractitemmodel.h"
#include "qvariant.h"
#include "qwidget.h"

// Set source ModuleLayers data
void ModuleLayersModel::setData(const std::vector<std::unique_ptr<ModuleLayer>> &layers, CoreData &coreData)
{
    coreData_ = coreData;

    beginResetModel();
    layers_ = layers;
    endResetModel();
}

ModuleLayer *ModuleLayersModel::rawData(const QModelIndex &index) const
{
    if (!layers_ || !index.isValid())
        return {};
    return layers_->get()[index.row()].get();
}

int ModuleLayersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return layers_ ? layers_->get().size() : 0;
}

void ModuleLayersModel::reset()
{
    beginResetModel();
    endResetModel();
}

QVariant ModuleLayersModel::data(const QModelIndex &index, int role) const
{

    auto data = rawData(index);
    if (!data)
        return {};

    switch (role)
    {
        case (Qt::DisplayRole):
            return QString::fromStdString(std::string(data->name()));
        case (static_cast<unsigned int>(ModuleLayersUserRole::RawData)):
            return QVariant::fromValue(data);
        case (static_cast<unsigned int>(ModuleLayersUserRole::ModuleLayerModel)):
        {
            auto *m = new ModuleLayerModel();
            m->setData(data, coreData_->get());
            return QVariant::fromValue(m);
        }
        default:
            return {};
    }
}

bool ModuleLayersModel::setData(const QModelIndex &index, const QVariant &value, int role) { return false; }

Qt::ItemFlags ModuleLayersModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

QVariant ModuleLayersModel::headerData(int section, Qt::Orientation orientation, int role) const { return {}; }
