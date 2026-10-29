// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "renderableModel.h"

//
void RenderableModel::update()
{
    for (auto &[_, model] : dataSets_)
        model->reset();
}

//
bool RenderableModel::empty() { return dataSets_.empty(); }

/*
 * QAbstractListModel overrides
 */

//
int RenderableModel::rowCount(const QModelIndex &parent) const { return dataSets_.size(); }

// Set source 1D data
bool RenderableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto renderable = qvariant_cast<RenderableData>(value);
    auto model = std::make_unique<Plot1DModel>(renderable.name, renderable.lambda);
    dataSets_.emplace_back(renderable.name, std::move(model));
    return true;
}

QVariant RenderableModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= rowCount())
        return {};

    switch (role - Qt::UserRole)
    {
        case NAME:
            return QString::fromStdString(dataSets_.at(row).first);
        case DATA:
            return QVariant::fromValue<Plot1DModel *>(dataSets_.at(row).second.get());
    }
    return {};
}

Qt::ItemFlags RenderableModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> RenderableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)NAME] = "name";
    roles[Qt::UserRole + (int)DATA] = "data";
    roles[Qt::UserRole + (int)TYPE] = "type";
    return roles;
}
