// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/speciesFilterProxy.h"
#include "classes/species.h"

Q_DECLARE_METATYPE(const Species *);

SpeciesFilterProxy::SpeciesFilterProxy(int flags) : filterFlags_(flags) {}

// Set filter flags
void SpeciesFilterProxy::setFlags(int flags)
{
    filterFlags_ = flags;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool SpeciesFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (filterFlags_ == SpeciesFilterProxy::None)
        return true;

    const auto *sp = sourceModel()->data(sourceModel()->index(row, 0, parent), Qt::UserRole).value<const Species *>();

    if (filterFlags_ & SpeciesFilterProxy::HasPeriodicBox && sp->box()->type() == Box::BoxType::NonPeriodic)
        return false;
    else if (filterFlags_ & SpeciesFilterProxy::NoPeriodicBox && sp->box()->type() != Box::BoxType::NonPeriodic)
        return false;

    return true;
}
