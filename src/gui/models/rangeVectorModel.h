// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "module/module.h"
#include "templates/optionalRef.h"
#include <QAbstractItemModel>
#include <QModelIndex>

#include <vector>

class RangeVectorModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    // Vector containing ranges
    OptionalReferenceWrapper<std::vector<Range>> ranges_;

    public:
    // Set ranges
    void setData(std::vector<Range> &ranges);
    // Returns range object from vector
    OptionalReferenceWrapper<Range> getRange(const QModelIndex &index) const;

    /*
     * QAbstractTableModel overrides
     */
    public:
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
};