// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QSortFilterProxyModel>
#include <bitset>

// Forward Declarations
class QModelIndex;

class SitesFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    SitesFilterProxy(int flags = 0);
    // Filter flags
    enum FilterFlags
    {
        HasAxes,
        nFilterFlags
    };

    private:
    // Current filter flags
    std::bitset<nFilterFlags> filterFlags_;

    public:
    // Set filter flag
    void setFlag(FilterFlags flag);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
