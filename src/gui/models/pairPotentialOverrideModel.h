// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/pairPotentialOverride.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <vector>

class PairPotentialOverrideModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source pair potential override data
    std::vector<std::unique_ptr<PairPotentialOverride>> &data_;

    public:
    // Column Data
    enum ColumnData
    {
        MatchI,
        MatchJ,
        OverrideType,
        ShortRangeForm,
        ShortRangeParameters,
        nColumnData
    };
    PairPotentialOverrideModel(std::vector<std::unique_ptr<PairPotentialOverride>> &data_);
    ~PairPotentialOverrideModel() = default;
    const PairPotentialOverride *rawData(const QModelIndex index) const;
    PairPotentialOverride *rawData(const QModelIndex index);
    // Update the table contents
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
