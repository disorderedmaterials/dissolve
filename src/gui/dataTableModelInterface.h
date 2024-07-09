// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/expressionVariableVector.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

// Qt Interface to DataTableModel
class DataTableModelInterface : public QAbstractTableModel
{
    Q_OBJECT

    public:
    DataTableModelInterface(DataModelBase &dataModel);

    private:
    // Model with which to interface
    DataModelBase &dataModel_;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
};
