// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/dataModelBase.h"
#include <QAbstractTableModel>
#include <QModelIndex>

// QAbstractTableModel Interface to DataModel::Table
class DataModelTableInterface : public QAbstractTableModel
{
    public:
    DataModelTableInterface(DataModel::Base &dataModel);
    ~DataModelTableInterface();

    private:
    // Model with which to interface
    DataModel::Base &dataModel_;

    /*
     * QAbstractTableModel Overrides
     */
    public:
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
    // Append one or more rows
    bool appendRows(int count, const QModelIndex &parent);
    // Remove one or more rows starting from the specified position
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    /*
     * Mutation Interface
     */
    private:
    // React to a mutation in the model
    void dataMutated(DataModel::Base::MutationSignal signal, int startIndex, int endIndex);
};
