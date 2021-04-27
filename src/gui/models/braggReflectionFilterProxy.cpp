// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/braggReflectionFilterProxy.h"

BraggReflectionFilterProxy::BraggReflectionFilterProxy() : enabled_(true) {}

// Set whether the filter is enabled
void BraggReflectionFilterProxy::setEnabled(bool b)
{
    enabled_ = b;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool BraggReflectionFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (!enabled_)
        return true;

    for (auto col = 3; col < sourceModel()->columnCount(); ++col)
        if (sourceModel()->data(sourceModel()->index(row, col, parent)).toDouble() > 1.0e-3)
            return true;

    return false;
}
