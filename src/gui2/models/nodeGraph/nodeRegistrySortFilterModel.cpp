// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/nodeGraph/nodeRegistrySortFilterModel.h"
#include "gui2/models/nodeGraph/nodeRegistryModel.h"
#include <algorithm>
#include <vector>

NodeRegistrySortFilterModel::NodeRegistrySortFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {}

bool NodeRegistrySortFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &source) const
{
    if (searchText_.isEmpty())
        return false;

    auto index = sourceModel()->index(sourceRow, 0, source);
    auto rowName = sourceModel()->data(index, NodeRegistryModel::Name).toString().toStdString();

    auto fuzzyTypes = NodeRegistry::getNodeTypesFuzzy(searchText_.toStdString());

    return std::find(fuzzyTypes.begin(), fuzzyTypes.end(), rowName) != fuzzyTypes.end();
}

QString &NodeRegistrySortFilterModel::searchText() { return searchText_; }

void NodeRegistrySortFilterModel::setSearchText(QString &text)
{
    // Expect 62 rows and 1 column in source model
    searchText_ = text;
    invalidateFilter();
    Q_EMIT searchTextChanged();
}