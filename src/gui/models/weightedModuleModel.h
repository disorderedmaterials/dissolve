// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "templates/optionalRef.h"
#include <QAbstractItemModel>
#include <QModelIndex>

#include <vector>

class WeightedModuleModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    private:
    // Source Module data
    OptionalReferenceWrapper<const std::vector<Module *>> modules_;
    // Vector containing selected/weighted items
    OptionalReferenceWrapper<std::vector<std::pair<Module *, double>>> weightedItems_;

    public:
    // Set source Module data
    void setData(const std::vector<Module *> &modules);
    // Set vector containing selected/weighted items
    void setWeightedItems(std::vector<std::pair<Module *, double>> &weightedItems);
    // Return object represented by specified model index
    Module *rawData(const QModelIndex &index) const;

    /*
     * QAbstractItemModel overrides
     */
    public:
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
