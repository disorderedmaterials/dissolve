// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

class SitesModel : public QAbstractListModel
{
    Q_OBJECT

    public:
    SitesModel(OptionalReferenceWrapper<const std::vector<std::unique_ptr<Species>>> species = std::nullopt,
               OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedSites = std::nullopt);

    private:
    // Source SpeciesSite data
    OptionalReferenceWrapper<const std::vector<std::unique_ptr<Species>>> species_;
    // Vector containing checked items (if relevant)
    OptionalReferenceWrapper<std::vector<const SpeciesSite *>> checkedItems_;
    // Return object represented by specified model index
    SpeciesSite *rawData(const QModelIndex &index) const;

    public:
    // Set source Species data
    void setData(const std::vector<std::unique_ptr<Species>> &species);
    // Set vector containing checked items
    void setCheckStateData(std::vector<const SpeciesSite *> &checkedItemsVector);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
