// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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

    return node->children().size();
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
    switch (role)
    {
        case Qt::DisplayRole:
            return QString::fromStdString(fmt::format("{}", node->nodeTypes().keyword(node->type())));
        case Qt::UserRole:
            return QVariant::fromValue(node->shared_from_this());
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
    if (!node)
    {
        return createIndex(row, column, procedure_.nodes()[row].get());
    }
    return createIndex(row, column, node->children()[row].get());
}

QModelIndex ProcedureModel::parent(const QModelIndex &index) const
{
    auto source = static_cast<ProcedureNode *>(index.internalPointer())->parent();
    if (!source)
        return QModelIndex();
    auto gp = source->parent();
    if (gp)
    {
        auto it = std::find(gp->children().begin(), gp->children().end(), source);
        return createIndex(it - gp->children().begin(), 0, gp.get());
    }
    else
    {
        auto it = std::find(procedure_.nodes().begin(), procedure_.nodes().end(), source);
        return createIndex(it - procedure_.nodes().begin(), 0, nullptr);
    }
}
