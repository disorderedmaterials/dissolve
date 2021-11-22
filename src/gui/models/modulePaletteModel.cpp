// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/modulePaletteModel.h"
#include "modules/registry.h"
#include <QIODevice>
#include <QIcon>
#include <QMimeData>

/*
 * QAbstractItemModel overrides
 */

int ModulePaletteModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return ModuleRegistry::categoryMap()[parent.row()].second.size();
    return ModuleRegistry::categoryMap().size();
}

int ModulePaletteModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

bool ModulePaletteModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QModelIndex ModulePaletteModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

QModelIndex ModulePaletteModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = parent.row() + 1;
    else
        return {};

    return createIndex(row, column, child);
}

QVariant ModulePaletteModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid())
    {
        if (index.column() != 1)
            return {};

        auto [moduleType, brief] = ModuleRegistry::categoryMap()[index.parent().row()].second[index.row()];
        if (role == Qt::DisplayRole)
            return QString::fromStdString(moduleType);
        else if (role == Qt::ToolTipRole)
            return QString::fromStdString(brief);
        else if (role == Qt::DecorationRole)
            return QIcon(
                (QPixmap(QString(":/modules/icons/modules_%1.svg").arg(QString::fromStdString(moduleType).toLower()))));
    }
    else if (role == Qt::DisplayRole && index.column() == 0)
        return QString::fromStdString(ModuleRegistry::categoryMap()[index.row()].first);
    return {};
}

Qt::ItemFlags ModulePaletteModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
    else
        return Qt::ItemIsEnabled;
}

QVariant ModulePaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Category";
            case 1:
                return "Module";
            default:
                return {};
        }

    return {};
}

Qt::DropActions ModulePaletteModel::supportedDragActions() const { return Qt::CopyAction; }

QStringList ModulePaletteModel::mimeTypes() const
{
    QStringList types;
    types << "application/dissolve.module.create";
    return types;
}

QMimeData *ModulePaletteModel::mimeData(const QModelIndexList &indexes) const
{
    auto *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    auto index = indexes.front();
    auto [moduleType, brief] = ModuleRegistry::categoryMap()[index.parent().row()].second[index.row()];
    stream << QString::fromStdString(moduleType);
    mimeData->setData("application/dissolve.module.create", encodedData);

    return mimeData;
}
