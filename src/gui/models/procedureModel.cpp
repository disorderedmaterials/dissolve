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
    Q_UNUSED(parent);

    return procedure_.nodes().size();
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    auto node = procedure_.nodes()[index.row()];
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
