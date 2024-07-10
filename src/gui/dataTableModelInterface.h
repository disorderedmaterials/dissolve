// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/expressionVariableVector.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

// QAbstractTableModel Interface to DataTableModel
class DataTableModelInterface : public QAbstractTableModel
{
    Q_OBJECT

    public:
    DataTableModelInterface(DataModelBase &dataModel);

    private:
    // Model with which to interface
    DataModelBase &dataModel_;

    /*
     * QAbstractTableModel Overrides
     */
    private:
    // Return row count
    int rowCount(const QModelIndex &parent) const override;
    // Return column count
    int columnCount(const QModelIndex &parent) const override;
    // Return flags for the specified model index
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    // Return header data for the specified section, orientation, and role
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    // Return data for the index and role specified
    QVariant data(const QModelIndex &index, int role) const override;
    // Set data for the index and role specified
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    // Insert one or more rows at the specified position
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    // Remove one or more rows starting from the specified position
    bool removeRows(int row, int count, const QModelIndex &parent) override;
};
