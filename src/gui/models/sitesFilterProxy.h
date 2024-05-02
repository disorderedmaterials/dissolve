// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class SitesFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    // Filter flags
    enum FilterFlags
    {
        HasAxes
    };

    private:
    // Current filter flags
    Flags<FilterFlags> flags_;

    public:
    // Set filter flag
    void setFlag(FilterFlags flag);
    // Remove filter flag
    void removeFlag(FilterFlags flag);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
