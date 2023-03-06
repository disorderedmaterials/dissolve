// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include <QSortFilterProxyModel>

class ForcefieldSortFilterModel : public QSortFilterProxyModel
{
    public:
    ForcefieldSortFilterModel(QObject *parent, CoreData &coreData);
    void setSpecies(const Species *species);

    protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &source) const override;

    private:
    CoreData &temporaryCoreData_;
    const Species *species_ = nullptr;
};
