// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/nodePaletteFilterProxy.h"
#include "procedure/nodes/registry.h"

NodePaletteFilterProxy::NodePaletteFilterProxy(const std::vector<std::string>& categories) : categories_(categories) {}

// Set allowed categories
void NodePaletteFilterProxy::setCategories(const std::vector<std::string>& categories)
{
    categories_ = categories;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool NodePaletteFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    auto category =
        std::next(ProcedureNodeRegistry::categoryMap().begin(), sourceModel()->parent(sourceModel()->index(row, 0)).row())->first;
    return std::find(categories_.begin(), categories_.end(), category) != categories_.end();
}

