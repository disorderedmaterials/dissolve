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
    //
    std::vector<std::pair<int, int>> indices_;
    // Dataset name
    const std::string &name_;
    //
    const Data2D *sourceData_;
    //
    std::string xName_{"x-axis"};
    //
    std::string yName_{"y-axis"};
    //
    std::string zName_{"z-axis"};
    //
    int currentNXValues_;
    //
    int currentNYValues_;

    private:
    //
    QString name();
    //
    double minX();
    //
    double minY();
    //
    double minZ();
    //
    double maxX();
    //
    double maxY();
    //
    double maxZ();
    //
    QString xName();
    //
    QString yName();
    //
    QString zName();
    //
    int currentNXValues();
    //
    int currentNYValues();

    private:
    //
    void handleDataChanged() override;
    //
    void regenerateIndices(int nX, int nY);

    public Q_SLOTS:
    //
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