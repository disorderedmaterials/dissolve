// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesimproper.h"
#include "data/ff/improperterm.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

// Forward Declarations
class CoreData;

class SpeciesImproperModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<SpeciesImproper> &impropers_;
    const CoreData &coreData_;

    public:
    SpeciesImproperModel(std::vector<SpeciesImproper> &impropers, const CoreData &coreData);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
