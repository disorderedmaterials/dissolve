// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesSiteFilterProxy.h"
#include "classes/speciesSite.h"

// Set filter flag
void SpeciesSiteFilterProxy::setFlag(SpeciesSiteFilterProxy::FilterFlag flag)
{
    flags_ += flag;
    invalidateFilter();
}

// Remove filter flag
void SpeciesSiteFilterProxy::removeFlag(SpeciesSiteFilterProxy::FilterFlag flag)
{
    flags_ -= flag;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool SpeciesSiteFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (!flags_.anySet())
        return true;

    const auto *site = sourceModel()->data(sourceModel()->index(row, 0, parent), Qt::UserRole).value<SpeciesSite *>();

    if (flags_.isSet(SpeciesSiteFilterProxy::FilterFlag::IsOriented) && !site->hasAxes())
        return false;

    if (flags_.isSet(SpeciesSiteFilterProxy::FilterFlag::HideDynamic) && site->type() == SpeciesSite::SiteType::Dynamic)
        return false;

    if (flags_.isSet(SpeciesSiteFilterProxy::FilterFlag::HideStatic) && site->type() == SpeciesSite::SiteType::Static)
        return false;

    return true;
}
