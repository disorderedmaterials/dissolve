// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/sitesFilterProxy.h"
#include "classes/speciessite.h"

Q_DECLARE_METATYPE(SpeciesSite *);

SitesFilterProxy::SitesFilterProxy(int flags) : filterFlags_(flags) {}

// Set filter flags
void SitesFilterProxy::setFlags(int flags)
{
    filterFlags_ = flags;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool SitesFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (filterFlags_ == SitesFilterProxy::None)
        return true;

    const auto *site = sourceModel()->data(sourceModel()->index(row, 0, parent), Qt::UserRole).value<SpeciesSite *>();

    if (filterFlags_ & SitesFilterProxy::IsOriented && !site->hasAxes())
        return false;

    return true;
}
