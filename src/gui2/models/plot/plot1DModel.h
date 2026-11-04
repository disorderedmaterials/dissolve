// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "nodes/node.h"
#include <QAbstractListModel>
#include <QVariantList>
#include <vector>

class Plot1DModel : public QAbstractListModel
{
    Q_OBJECT;

    public:
    Plot1DModel(const std::string &name, const std::function<const Data1D *()> &sourceDataGetter);
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
    const std::function<const Data1D *()> sourceDataGetter_;
    //
    std::string xName_{"x-axis"};
    //
    std::string yName_{"y-axis"};

    public:
    //
    void reset();

    /*
     * QAbstractListModel overrides
     */

    public:
    // Number of rows
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const;

    public Q_SLOTS:
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
    //
    double xOffsetPercentage();
    //
    double yOffsetPercentage();

    Q_SIGNALS:
    //
    void dataChanged();
};