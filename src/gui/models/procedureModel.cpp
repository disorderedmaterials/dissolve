// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/procedureModel.h"
#include "procedure/procedure.h"

/*
 * QAbstractItemModel overrides
 */
ProcedureModel::ProcedureModel(Procedure &procedure) : procedure_(procedure) {}

int ProcedureModel::rowCount(const QModelIndex &parent) const
{
    auto node = static_cast<ProcedureNode *>(parent.internalPointer());

    if (!node)
        return procedure_.nodes().size();

    return node->scope()->sequence().size();
}

int ProcedureModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::UserRole)
        return {};
    auto node = static_cast<ProcedureNode *>(index.internalPointer());
    if (!node)
    {
        return "Foo";
    }
    switch (role)
    {
        case Qt::DisplayRole:
            return QString::fromStdString(fmt::format("{}", node->nodeTypes().keyword(node->type())));
        case Qt::UserRole:
            return QVariant::fromValue(node);
        default:
            return {};
    }
}

QVariant ProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        return "Name";

    return {};
}

QModelIndex ProcedureModel::index(int row, int column, const QModelIndex &parent) const
{
    auto node = static_cast<ProcedureNode *>(parent.internalPointer());
    if (!parent.isValid() || !node)
    {
        return createIndex(row, column, procedure_.nodes()[row].get());
    }
    return createIndex(row, column, node->scope()->sequence()[row].get());
}

QModelIndex ProcedureModel::parent(const QModelIndex &index) const
{
    auto source = static_cast<ProcedureNode *>(index.internalPointer());
    auto gp = source->parent();
    if (!gp)
    {
        auto it = std::find_if(procedure_.nodes().begin(), procedure_.nodes().end(),
                               [source](const auto n) { return n.get() == source; });
        int row = it - procedure_.nodes().begin();
        return createIndex(row, 0, nullptr);
    }
    auto it = std::find_if(gp->nodesInScope().begin(), gp->nodesInScope().end(),
                           [source](const auto n) { return n.get() == source; });
    return createIndex(it - gp->nodesInScope().begin(), 0, gp.get());
}
