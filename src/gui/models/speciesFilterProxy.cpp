// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesFilterProxy.h"
#include "classes/species.h"

SpeciesFilterProxy::SpeciesFilterProxy(const Flags<SpeciesFilterProxy::FilterFlags> &flags) : filterFlags_(flags) {}

// Set filter flags
void SpeciesFilterProxy::setFlags(const Flags<SpeciesFilterProxy::FilterFlags> &flags)
{
    filterFlags_ = flags;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool SpeciesFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (!filterFlags_.anySet())
        return true;

    const auto *sp = sourceModel()->data(sourceModel()->index(row, 0, parent), Qt::UserRole).value<const Species *>();

    if (filterFlags_.isSet(SpeciesFilterProxy::HasPeriodicBox) && sp->box()->type() == Box::BoxType::NonPeriodic)
        return false;
    else if (filterFlags_.isSet(SpeciesFilterProxy::NoPeriodicBox) && sp->box()->type() != Box::BoxType::NonPeriodic)
        return false;

    return true;
}
