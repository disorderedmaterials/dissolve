// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <QAbstractTableModel>

class PlotModelBase : public QAbstractTableModel
{
    Q_OBJECT;

    public:
    PlotModelBase(int dimensions, QObject *parent = nullptr);
    ~PlotModelBase() = default;

    public:
    //
    const int N;

    public:
    //
    virtual void handleDataChanged() = 0;

    /*
     * QAbstractListModel overrides
     */

    public:
    // Number of rows
    int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    // Number of columns
    int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const = 0;
};