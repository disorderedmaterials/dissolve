// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/renderableGroupManagerModel.h"
#include "gui/render/renderable.h"

RenderableGroupManagerModel::RenderableGroupManagerModel(OptionalReferenceWrapper<RenderableGroupManager> manager)
    : manager_(manager)
{
}

void RenderableGroupManagerModel::setSourceData(RenderableGroupManager &manager)
{
    beginResetModel();
    manager_ = manager;
    endResetModel();
}

int RenderableGroupManagerModel::rowCount(const QModelIndex &parent) const
{
    if (!manager_)
        return 0;

    if (!parent.isValid())
        return manager_->get().groups().size();
    return manager_->get().groups()[parent.row()].renderables().size();
}

int RenderableGroupManagerModel::columnCount(const QModelIndex &parent) const { return 1; }

QModelIndex RenderableGroupManagerModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return {};
    return createIndex(index.internalId() - 1, 0, root);
}

bool RenderableGroupManagerModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QVariant RenderableGroupManagerModel::data(const QModelIndex &index, int role) const
{
    if (!manager_ || index.column() != 0)
        return {};

    const auto &set = manager_->get();

    // Root item (RenderableGroup)
    if (!index.parent().isValid())
    {
        if (index.row() > set.groups().size())
            return {};

        const auto &group = set.groups()[index.row()];

        switch (role)
        {
            case (Qt::DisplayRole):
            case (Qt::EditRole):
                if (index.column() == 0)
                    return QString::fromStdString(std::string(group.name()));
                else
                    return {};
            case (Qt::CheckStateRole):
                return group.isVisible() ? Qt::Checked : Qt::Unchecked;
            case (Qt::UserRole):
                return QVariant::fromValue(&group);
            default:
                return {};
        }
    }

    // Secondary item (Renderable)
    const auto &group = set.groups()[index.parent().row()];
    const auto renderable = group.renderables()[index.row()];
    switch (role)
    {
        case (Qt::DisplayRole):
        case (Qt::EditRole):
            return QString::fromStdString(std::string(renderable->name()));
        case (Qt::CheckStateRole):
            return renderable->isVisible() ? Qt::Checked : Qt::Unchecked;
        case (Qt::UserRole):
            return QVariant::fromValue(renderable);
        default:
            return {};
    }

    return {};
}

QModelIndex RenderableGroupManagerModel::index(int row, int column, const QModelIndex &parent) const
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

Qt::ItemFlags RenderableGroupManagerModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

bool RenderableGroupManagerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Only editability is the check state of the items
    if (!manager_ || role != Qt::CheckStateRole)
        return false;

    auto &manager = manager_->get();

    // If there is no parent then a group was (un)checked - otherwise it was a specific renderable
    if (index.parent().isValid())
    {
        auto renderable = manager.groups()[index.parent().row()].renderables()[index.row()];
        if (!renderable)
            return false;

        renderable->setVisible(value.toBool());

        Q_EMIT(dataChanged(index, index));
        return true;
    }
    else
    {
        auto &group = manager.groups()[index.row()];

        group.setVisible(value.toBool());

        Q_EMIT(dataChanged(index, index));
        return true;
    }

    return false;
}
