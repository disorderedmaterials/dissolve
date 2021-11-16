// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/layer.h"
#include <QAbstractListModel>
#include <QModelIndex>

class ModuleLayerModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source ModuleLayer
    ModuleLayer *moduleLayer_{nullptr};
    // Return object represented by specified model index
    Module *rawData(const QModelIndex &index) const;

    public:
    // Set source Species data
    void setData(ModuleLayer *moduleLayer);

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
