// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/moduleLayerModel.h"
#include "module/module.h"
#include <QIcon>

// Set source Species data
void ModuleLayerModel::setData(ModuleLayer *moduleLayer)
{
    beginResetModel();
    moduleLayer_ = moduleLayer;
    endResetModel();
}

Module *ModuleLayerModel::rawData(const QModelIndex &index) const
{
    assert(moduleLayer_);
    return moduleLayer_->modules()[index.row()].get();
}

/*
 * QAbstractItemModel overrides
 */

int ModuleLayerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return moduleLayer_ ? moduleLayer_->nModules() : 0;
}

QVariant ModuleLayerModel::data(const QModelIndex &index, int role) const
{
    auto *module = rawData(index);

    if (role == Qt::DisplayRole)
    {
        if (module->isDisabled())
            return QString::fromStdString(fmt::format("{} (disabled)", module->uniqueName()));
        else if (!moduleLayer_->isEnabled())
            return QString::fromStdString(fmt::format("{} (disabled via layer)", module->uniqueName()));
        else
            return QString::fromStdString(std::string(module->uniqueName()));
    }
    else if (role == Qt::EditRole)
        return QString::fromStdString(std::string(module->uniqueName()));
    else if (role == Qt::UserRole)
        return QVariant::fromValue(module);
    else if (role == Qt::DecorationRole)
        return QIcon((QPixmap(
            QString(":/modules/icons/modules_%1.svg").arg(QString::fromStdString(std::string(module->type())).toLower()))));
    return {};
}

bool ModuleLayerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        moduleLayer_->modules()[index.row()]->setUniqueName(value.toString().toStdString());

        emit dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags ModuleLayerModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
}

QVariant ModuleLayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            default:
                return {};
        }

    return {};
}
