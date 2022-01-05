// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class SpeciesSiteFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    SpeciesSiteFilterProxy(int flags = SpeciesSiteFilterProxy::None);

    public:
    // Filter flags
    enum FilterFlags
    {
        None = 0,
        IsOriented = 1
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
