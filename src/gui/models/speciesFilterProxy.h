// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class SpeciesFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    // Filter flags
    enum FilterFlags
    {
        HasPeriodicBox = 0,
        NoPeriodicBox = 1
    };
    SpeciesFilterProxy(const Flags<FilterFlags> &flags = {});

    private:
    // Current filter flags
    Flags<FilterFlags> filterFlags_;

    public:
    // Set filter flags
    void setFlags(const Flags<FilterFlags> &flags);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
