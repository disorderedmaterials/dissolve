// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/sitesFilterProxy.h"
#include "classes/speciesSite.h"

// Set filter flag
void SitesFilterProxy::setFlag(SitesFilterProxy::FilterFlags flag)
{
    flags_ += flag;
    invalidateFilter();
}

// Remove filter flag
void SitesFilterProxy::removeFlag(SitesFilterProxy::FilterFlags flag)
{
    flags_ -= flag;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool SitesFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (!flags_.anySet() || !parent.isValid())
        return true;

    const auto *site = sourceModel()->data(sourceModel()->index(row, 1, parent), Qt::UserRole).value<SpeciesSite *>();
    assert(site);

    if (flags_.isSet(SitesFilterProxy::HasAxes) && !site->hasAxes())
        return false;

    return true;
}
