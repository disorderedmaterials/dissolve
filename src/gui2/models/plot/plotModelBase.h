// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <array>

class PlotModelBase : public QAbstractListModel
{
    Q_OBJECT;

    Q_PROPERTY(int version READ version NOTIFY contentChanged);

    public:
    PlotModelBase(int dimensions, QObject *parent = nullptr);
    ~PlotModelBase() = default;

    private:
    //
    int version_{0};

    public:
    //
    const int N;

    public:
    //
    virtual void handleDataChanged() = 0;

    Q_SIGNALS:
    //
    void contentChanged();

    private:
    //
    int version();

    /*
     * QAbstractListModel overrides
     */

    public:
    // Number of rows
    int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const = 0;
};