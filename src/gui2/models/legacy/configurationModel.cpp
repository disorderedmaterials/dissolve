// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/configurationModel.h"
#include "nodes/structure/configuration.h"
#include <iostream>
#include <qvariant.h>

/*
 * QAbstractItemModel overrides
 */

int ConfigurationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return root_->findType<ConfigurationNode>().size();
}

QVariant ConfigurationModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (!index.isValid())
        return {};

    auto nodes = root_->findType<ConfigurationNode>();
    if (index.row() >= nodes.size())
        return {};
    return QString::fromStdString(std::string(nodes[index.row()]->name()));
}

Graph *ConfigurationModel::graph() const { return root_; }

void ConfigurationModel::setGraph(Graph *node)
{
    beginResetModel();
    root_ = node;
    endResetModel();
}

void ConfigurationModel::reset()
{
    beginResetModel();
    endResetModel();
}
