// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/atomTypeFilterProxy.h"
#include "classes/atomType.h"
#include "data/elements.h"

// Set filter element
void AtomTypeFilterProxy::setFilterElement(Elements::Element Z)
{
    filterElement_ = Z;
    invalidateFilter();
}

// Clear filter element
void AtomTypeFilterProxy::clearFilterElement()
{
    filterElement_ = std::nullopt;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool AtomTypeFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (!filterElement_)
        return true;

    const auto at = sourceModel()->data(sourceModel()->index(row, 0, parent), Qt::UserRole).value<std::shared_ptr<AtomType>>();
    return at->Z() == filterElement_.value();
}
