// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include <QSortFilterProxyModel>

// Forward Declarations
class QModelIndex;

class AtomTypeFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    AtomTypeFilterProxy() = default;

    private:
    // Filter element
    std::optional<Elements::Element> filterElement_;

    public:
    // Set filter element
    void setFilterElement(Elements::Element Z);
    // Clear filter element
    void clearFilterElement();

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};
