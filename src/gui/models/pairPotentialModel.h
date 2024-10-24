// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/pairPotential.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>

#include <vector>

class PairPotentialModel : public QAbstractListModel
{
    Q_OBJECT

    public:
    PairPotentialModel(const std::vector<PairPotential::Definition> &data);
    ~PairPotentialModel() = default;

    enum Columns
    {
        NameIColumn,
        NameJColumn,
        ChargeProductColumn,
        ShortRangeFormColumn,
        ShortRangeParametersColumn,
        nDataColumns
    };

    private:
    // Source pair potential data
    const std::vector<PairPotential::Definition> &data_;
    // Whether the pair potential is editable
    bool editable_{false};

    public:
    // Set whether the data is editable or not
    void setEditable(bool b);
    // Retrieve raw data for model
    const PairPotential *rawData(const QModelIndex index) const;
    PairPotential *rawData(const QModelIndex index);
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
