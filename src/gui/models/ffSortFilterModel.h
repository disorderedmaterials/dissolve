// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include <QSortFilterProxyModel>

class ForcefieldSortFilterModel : public QSortFilterProxyModel
{
    public:
    ForcefieldSortFilterModel(QObject *parent, const CoreData &coreData);
    void setSpecies(const Species *species);

    protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &source) const override;

    private:
    const CoreData &coreData_;
    const Species *species_ = nullptr;
};
