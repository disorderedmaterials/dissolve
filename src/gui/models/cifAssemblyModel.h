// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/import/cif.h"
#include <QAbstractItemModel>
#include <QModelIndex>

class CIFAssemblyModel : public QAbstractItemModel
{
    Q_OBJECT
    private:
    std::vector<CIFAssembly> &assemblies_;

    public:
    CIFAssemblyModel(std::vector<CIFAssembly> &assemblies);

    QModelIndex parent(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    bool hasChildren(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
