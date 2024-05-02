// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/forcefieldModel.h"

ForcefieldModel::ForcefieldModel(const std::vector<std::shared_ptr<Forcefield>> &forcefields) : forcefields_(forcefields) {}

int ForcefieldModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return forcefields_.size();
}

QVariant ForcefieldModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > forcefields_.size())
        return {};

    switch (role)
    {
        case Qt::DisplayRole: // Intentional Fallthrough
        case NameRole:
            return QString::fromStdString(std::string(forcefields_[index.row()]->name()));
        case DescRole:
            return QString::fromStdString(std::string(forcefields_[index.row()]->description()));
        case RawRole:
            return QVariant::fromValue(forcefields_[index.row()].get());
        default:
            return {};
    }
}

QHash<int, QByteArray> ForcefieldModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "display";
    roles[DescRole] = "description";
    roles[RawRole] = "raw";
    return roles;
}
