// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAtom.h"
#include "gui/models/readVectorModel.h"
#include "main/dissolve.h"
#include <QAbstractTableModel>
#include <QModelIndex>

class SpeciesAtomModel : public ReadVectorModel<SpeciesAtom>
{
    Q_OBJECT

    Q_SIGNALS:
    void atomTypeChanged();

    public:
    SpeciesAtomModel(Species &species, const CoreData &coreData);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void clear();
};
