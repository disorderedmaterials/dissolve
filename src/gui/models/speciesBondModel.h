// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/speciesbond.h"
#include <QAbstractTableModel>
#include <QModelIndex>

// Forward Declarations
class CoreData;

class SpeciesBondModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<SpeciesBond> &bonds_;
    const CoreData &coreData_;

    public:
    SpeciesBondModel(std::vector<SpeciesBond> &bonds, const CoreData &coreData);
    void reset();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
