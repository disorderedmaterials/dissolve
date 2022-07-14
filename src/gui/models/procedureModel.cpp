// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/procedureModel.h"
#include "procedure/procedure.h"
#include <QIcon>

ProcedureModel::ProcedureModel(OptionalReferenceWrapper<Procedure> procedure) : procedure_(procedure) {}

// Set source Procedure
void ProcedureModel::setData(Procedure &procedure)
{
    beginResetModel();
    procedure_ = procedure;
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int ProcedureModel::rowCount(const QModelIndex &parent) const
{
    if (!procedure_)
        return 0;

    auto node = static_cast<ProcedureNode *>(parent.internalPointer());

    if (!node)
        return procedure_->get().nodes().size();

    return node->children().size();
}

int ProcedureModel::columnCount(const QModelIndex &parent) const
{
    if (!procedure_)
        return 0;

    Q_UNUSED(parent);

    return 1;
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    if (!procedure_)
        return {};

    auto node = static_cast<ProcedureNode *>(index.internalPointer());
    switch (role)
    {
        case Qt::DisplayRole:
            if (node->name().empty())
                return QString::fromStdString(std::string(node->nodeTypes().keyword(node->type())));
            else
                return QString("%1 (%2)").arg(
                    QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->type()))),
                    QString::fromStdString(std::string(node->name())));
        case Qt::UserRole:
            return QVariant::fromValue(node->shared_from_this());
        case Qt::DecorationRole:
            return QIcon((QPixmap(
                QString(":/nodes/icons/nodes_%1.svg")
                    .arg(QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->type()))).toLower()))));
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
    if (!procedure_)
        return {};

    auto node = static_cast<ProcedureNode *>(parent.internalPointer());
    if (!node)
        return createIndex(row, column, procedure_->get().nodes()[row].get());

    return createIndex(row, column, node->children()[row].get());
}

QModelIndex ProcedureModel::parent(const QModelIndex &index) const
{
    if (!procedure_)
        return {};

    auto source = static_cast<ProcedureNode *>(index.internalPointer())->parent();
    if (!source)
        return {};
    auto gp = source->parent();
    if (gp)
    {
        auto it = std::find(gp->children().begin(), gp->children().end(), source);
        return createIndex(it - gp->children().begin(), 0, gp.get());
    }
    else
    {
        auto &proc = procedure_->get();
        auto it = std::find(proc.nodes().begin(), proc.nodes().end(), source);
        return createIndex(it - proc.nodes().begin(), 0, nullptr);
    }
}
