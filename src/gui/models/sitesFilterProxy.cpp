// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/sitesFilterProxy.h"
#include "classes/speciessite.h"

SitesFilterProxy::SitesFilterProxy(int flags) : filterFlags_(flags) {}

// Set filter flag
void SitesFilterProxy::setFlag(SitesFilterProxy::FilterFlags flag)
{
    filterFlags_.set(flag);
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool SitesFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (filterFlags_.none() || !parent.isValid())
        return true;

    const auto *site = sourceModel()->data(sourceModel()->index(row, 1, parent), Qt::UserRole).value<SpeciesSite *>();
    assert(site);

    if (filterFlags_.test(SitesFilterProxy::HasAxes) && !site->hasAxes())
        return false;

    return true;
}
