// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class SpeciesFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    SpeciesFilterProxy(int flags = SpeciesFilterProxy::None);

    public:
    // Filter flags
    enum FilterFlags
    {
        None = 0,
        HasPeriodicBox = 1,
        NoPeriodicBox = 2
    };
    // Current filter flags
    int filterFlags_;
    // Set filter flags
    void setFlags(int flags);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
