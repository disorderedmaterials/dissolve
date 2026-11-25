// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/plot/plotModelBase.h"
#include "math/data1D.h"
#include "nodes/node.h"
#include <QAbstractListModel>
#include <QVariantList>
#include <vector>

class Plot1DModel : public PlotModelBase
{
    Q_OBJECT;

    Q_PROPERTY(QString name READ name NOTIFY contentChanged)
    Q_PROPERTY(double minX READ minX NOTIFY contentChanged)
    Q_PROPERTY(double minY READ minY NOTIFY contentChanged)
    Q_PROPERTY(double maxX READ maxX NOTIFY contentChanged)
    Q_PROPERTY(double maxY READ maxY NOTIFY contentChanged)
    Q_PROPERTY(QString xName READ xName NOTIFY contentChanged)
    Q_PROPERTY(QString yName READ yName NOTIFY contentChanged)

    public:
    Plot1DModel(const std::string &name, const Data1D *sourceData);
    ~Plot1DModel() = default;

    enum Roles
    {
        X,
        Y
    };

    private:
    // Dataset name
    const std::string &name_;
    //
    const Data1D *sourceData_;
    //
    std::string xName_{"x-axis"};
    //
    std::string yName_{"y-axis"};

    private:
    //
    QString name();
    //
    double minX();
    //
    double minY();
    //
    double maxX();
    //
    double maxY();
    //
    QString xName();
    //
    QString yName();

    private:
    //
    void handleDataChanged() override;

    /*
     * QAbstractTableModel overrides
     */

    public:
    // Number of rows
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Number of columns
    // int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
};