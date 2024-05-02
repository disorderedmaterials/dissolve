// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "templates/optionalRef.h"
#include <QAbstractListModel>
#include <QModelIndex>

#include <vector>

class ModuleModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source Module data
    OptionalReferenceWrapper<const std::vector<Module *>> modules_;
    // Vector containing checked items (if relevant)
    OptionalReferenceWrapper<std::vector<Module *>> checkedItems_;

    public:
    // Set source Module data
    void setData(const std::vector<Module *> &modules);
    // Set vector containing checked items
    void setCheckStateData(std::vector<Module *> &checkedItemsVector);
    // Return object represented by specified model index
    Module *rawData(const QModelIndex &index) const;

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
