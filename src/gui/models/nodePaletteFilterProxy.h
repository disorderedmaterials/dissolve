// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <QSortFilterProxyModel>

class NodePaletteFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    NodePaletteFilterProxy(const std::vector<std::string>& categories = {});

    private:
    // Currently allowed categories
    std::vector<std::string> categories_;

    public:
    // Set allowed categories
    void setCategories(const std::vector<std::string>& categories);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};