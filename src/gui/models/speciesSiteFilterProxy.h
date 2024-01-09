// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class SpeciesSiteFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    // Filter flags
    enum FilterFlag
    {
        IsOriented,
        HideDynamic,
        HideStatic
    };

    private:
    // Filter flags
    Flags<FilterFlag> flags_;

    public:
    // Set filter flag
    void setFlag(FilterFlag flag);
    // Remove filter flag
    void removeFlag(FilterFlag flag);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
