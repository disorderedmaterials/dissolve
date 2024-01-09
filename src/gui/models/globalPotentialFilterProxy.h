// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QSortFilterProxyModel>

class GlobalPotentialFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsColumn(int column, const QModelIndex &parent) const;
};