// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class BraggReflectionFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    BraggReflectionFilterProxy();

    private:
    // Whether the filter is enabled
    bool enabled_;

    public:
    // Set whether the filter is enabled
    void setEnabled(bool b);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
