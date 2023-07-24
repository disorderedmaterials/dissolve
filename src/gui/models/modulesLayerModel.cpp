// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/moduleLayersModel.h"
#include "qabstractitemmodel.h"
#include "qvariant.h"
#include "qwidget.h"

// Set source ModuleLayers data
void ModuleLayersModel::setData(const std::vector<std::unique_ptr<ModuleLayer>> &layers)
{
    beginResetModel();
    layers_ = layers;
    endResetModel();
}

ModuleLayer *ModuleLayersModel::rawData(const QModelIndex &index) const { return layers_->get()[index.row()].get(); }

int ModuleLayersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return layers_ ? layers_->get().size() : 0;
}

QVariant ModuleLayersModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case (Qt::DisplayRole):
            return QString::fromStdString(std::string(rawData(index)->name()));
        case (Qt::UserRole):
            return QVariant::fromValue(rawData(index));
        default:
            return {};
    }
}

bool ModuleLayersModel::setData(const QModelIndex &index, const QVariant &value, int role) { return false; }

Qt::ItemFlags ModuleLayersModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

QVariant ModuleLayersModel::headerData(int section, Qt::Orientation orientation, int role) const { return {}; }
