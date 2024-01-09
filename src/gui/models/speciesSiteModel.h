// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <vector>

class SpeciesSiteModel : public QAbstractListModel
{
    Q_OBJECT

    public:
    SpeciesSiteModel(OptionalReferenceWrapper<std::vector<std::unique_ptr<SpeciesSite>>> sites = std::nullopt,
                     OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItems = std::nullopt);

    private:
    // Source SpeciesSite data
    OptionalReferenceWrapper<std::vector<std::unique_ptr<SpeciesSite>>> sites_;
    // Vector containing checked items (if relevant)
    OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItems_;
    // Return object represented by specified model index
    SpeciesSite *rawData(const QModelIndex &index) const;

    public:
    // Set source SpeciesSite data
    void setData(OptionalReferenceWrapper<std::vector<std::unique_ptr<SpeciesSite>>> sites);
    // Set vector containing checked items
    void setCheckStateData(OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItemsVector);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
