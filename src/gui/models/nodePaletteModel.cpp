// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/nodePaletteModel.h"
#include "generator/registry.h"
#include "gui/models/generatorModelMimeData.h"
#include <QIODevice>
#include <QIcon>
#include <QMimeData>

/*
 * QAbstractItemModel overrides
 */

int NodePaletteModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return std::next(GeneratorNodeRegistry::categoryMap().begin(), parent.row())->second.size();
    return GeneratorNodeRegistry::categoryMap().size();
}

int NodePaletteModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

bool NodePaletteModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QModelIndex NodePaletteModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

QModelIndex NodePaletteModel::index(int row, int column, const QModelIndex &parent) const
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

QVariant NodePaletteModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid())
    {
        if (index.column() != 1)
            return {};

        auto [nodeType, brief] =
            std::next(GeneratorNodeRegistry::categoryMap().begin(), index.parent().row())->second[index.row()];
        switch (role)
        {
            case (Qt::DisplayRole):
                return QString::fromStdString(GeneratorNode::nodeTypes().keyword(nodeType));
            case (Qt::ToolTipRole):
                return QString::fromStdString(brief);
            case (Qt::DecorationRole):
                return QIcon(QPixmap(QString(":/nodes/icons/nodes/%1.svg")
                                         .arg(QString::fromStdString(GeneratorNode::nodeTypes().keyword(nodeType)))));
            default:
                return {};
        }
    }
    else if (role == Qt::DisplayRole && index.column() == 0)
        return QString::fromStdString(std::next(GeneratorNodeRegistry::categoryMap().begin(), index.row())->first);
    return {};
}

Qt::ItemFlags NodePaletteModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
    else
        return Qt::ItemIsEnabled;
}

QVariant NodePaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Category";
            case 1:
                return "Node";
            default:
                return {};
        }

    return {};
}

Qt::DropActions NodePaletteModel::supportedDragActions() const { return Qt::CopyAction; }

QStringList NodePaletteModel::mimeTypes() const
{
    QStringList types;
    types << "application/dissolve.procedure.newNode";
    return types;
}

QMimeData *NodePaletteModel::mimeData(const QModelIndexList &indexes) const
{
    auto index = indexes.front();
    auto [nodeType, brief] = std::next(GeneratorNodeRegistry::categoryMap().begin(), index.parent().row())->second[index.row()];
    auto *mimeData = new GeneratorModelMimeData(nodeType);

    return mimeData;
}
