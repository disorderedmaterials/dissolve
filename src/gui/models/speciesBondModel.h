// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/speciesBond.h"
#include <QAbstractTableModel>
#include <QModelIndex>

class SpeciesBondModel : public QAbstractTableModel
{
    Q_OBJECT

    // Enumerated column-to-data relationships
    enum DataType
    {
        IndexI = 0,
        IndexJ = 1,
        Form = 2,
        Parameters = 3,
        nDataTypes = 4
    };

    private:
    std::vector<SpeciesBond> *bonds_;

    public:
    SpeciesBondModel();
    void reset();
    void setBonds(std::vector<SpeciesBond> &bonds);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    friend class SpeciesModel;
};
