// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/plot/plotModelBase.h"
#include "math/data2D.h"
#include "nodes/node.h"
#include <QAbstractListModel>
#include <QVariantList>
#include <vector>

class Plot2DModel : public PlotModelBase
{
    Q_OBJECT;

    Q_PROPERTY(QString name READ name NOTIFY contentChanged)
    Q_PROPERTY(double minX READ minX NOTIFY contentChanged)
    Q_PROPERTY(double minY READ minY NOTIFY contentChanged)
    Q_PROPERTY(double minZ READ minZ NOTIFY contentChanged)
    Q_PROPERTY(double maxX READ maxX NOTIFY contentChanged)
    Q_PROPERTY(double maxY READ maxY NOTIFY contentChanged)
    Q_PROPERTY(double maxZ READ maxZ NOTIFY contentChanged)
    Q_PROPERTY(int nX READ currentNXValues NOTIFY contentChanged)
    Q_PROPERTY(int nY READ currentNYValues NOTIFY contentChanged)
    Q_PROPERTY(QString xName READ xName NOTIFY contentChanged)
    Q_PROPERTY(QString yName READ yName NOTIFY contentChanged)
    Q_PROPERTY(QString zName READ zName NOTIFY contentChanged)

    public:
    Plot2DModel(const std::string &name, const Data2D *sourceData);
    ~Plot2DModel() = default;

    enum Roles
    {
        INDEX,
    };

    private:
    // Indexes of the x-y plane
    std::vector<std::pair<int, int>> indices_;
    // Dataset name
    const std::string &name_;
    // Source data
    const Data2D *sourceData_;
    // X-axis name
    std::string xName_{"x-axis"};
    // Y-axis name
    std::string yName_{"y-axis"};
    // Z-axis name
    std::string zName_{"z-axis"};
    // Current number of x-axis values
    int currentNXValues_;
    // Current number of y-axis values
    int currentNYValues_;

    private:
    // Return the name of the dataset being plotted
    QString name();
    // Return the minimum x-value in the dataset
    double minX();
    // Return the minimum y-value in the dataset
    double minY();
    // Return the minimum z-value in the dataset
    double minZ();
    // Return the maximum x-value in the dataset
    double maxX();
    // Return the maximum y-value in the dataset
    double maxY();
    // Return the maximum z-value in the dataset
    double maxZ();
    // Return the name of the x-axis
    QString xName();
    // Return the name of the y-axis
    QString yName();
    // Return the name of the z-axis
    QString zName();
    // Return the current number of x-axis values
    int currentNXValues();
    // Return the current number of y-axis values
    int currentNYValues();

    public:
    // Return the source data pointer
    const Data2D *sourceData() const;

    private:
    // Perform any required updates to the model when the data is changed
    void handleDataChanged() override;
    // Generate the indices for the x-y plane, given the numbe rof x- and y-axis values
    void generateIndices(int nX, int nY);

    public Q_SLOTS:
    // Returns the z value at a given x-y point
    double z(int x, int y);

    /*
     * QAbstractTableModel overrides
     */

    public:
    // Number of rows
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
};