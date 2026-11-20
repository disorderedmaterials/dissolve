// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plot2DModel.h"
#include <algorithm>
#include <cmath>

Plot2DModel::Plot2DModel(const std::string &name, const std::function<const Data2D *()> &sourceDataGetter)
    : PlotModelBase(2), name_(name), sourceDataGetter_(sourceDataGetter)
{
}

//
QString Plot2DModel::name() { return QString::fromStdString(name_); }

//
double Plot2DModel::minX()
{
    const auto &x = sourceDataGetter_()->xAxis();
    if (x.empty())
        return 0;
    return *std::min_element(x.begin(), x.end());
}

//
double Plot2DModel::minY()
{
    const auto &y = sourceDataGetter_()->yAxis();
    if (y.empty())
        return 0;
    return *std::min_element(y.begin(), y.end());
}

//
double Plot2DModel::minZ() { return sourceDataGetter_()->minValue(); }

//
double Plot2DModel::maxX()
{
    const auto &x = sourceDataGetter_()->xAxis();
    if (x.empty())
        return 0;
    auto max = *std::max_element(x.cbegin(), x.cend());
    return max;
}

//
double Plot2DModel::maxY()
{
    const auto &y = sourceDataGetter_()->yAxis();
    if (y.empty())
        return 0;
    auto max = *std::max_element(y.cbegin(), y.cend());
    return max;
}

//
double Plot2DModel::maxZ() { return sourceDataGetter_()->maxValue(); }

//
QString Plot2DModel::xName() { return QString::fromStdString(xName_); }

//
QString Plot2DModel::yName() { return QString::fromStdString(yName_); }

//
QString Plot2DModel::zName() { return QString::fromStdString(zName_); }

//
void Plot2DModel::handleDataChanged()
{
    const auto &source = sourceDataGetter_();
    const auto n = source->nValues();
    for (int i = 0; i < n; i++)
    {
        auto index = this->index(i, 0);
        Q_EMIT dataChanged(index, index, {Qt::UserRole + X});
        Q_EMIT dataChanged(index, index, {Qt::UserRole + Y});
        Q_EMIT dataChanged(index, index, {Qt::UserRole + Z});
    }
    Q_EMIT contentChanged();
}

/*
 * QAbstractTableModel overrides
 */

// Number of rows
int Plot2DModel::rowCount(const QModelIndex &parent) const { return sourceDataGetter_()->yAxis().size(); }

// Number of rows
int Plot2DModel::columnCount(const QModelIndex &parent) const { return sourceDataGetter_()->xAxis().size(); }

// Access edge by QModelIndex.  The correct role can be found in the roleNames function.
QVariant Plot2DModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    auto col = index.column();

    if (row >= rowCount())
        return {};

    if (col >= columnCount())
        return {};

    switch (role - Qt::UserRole)
    {
        case X:
        {
            if (col >= 1)
                return {};
            return sourceDataGetter_()->xAxis()[row];
        }
        case Y:
        {
            if (row >= 1)
                return {};
            return sourceDataGetter_()->yAxis()[row];
        }
        case Z:
            return sourceDataGetter_()->values()[{row, column}];
    }
    return {};
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> Plot2DModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)X] = "pX";
    roles[Qt::UserRole + (int)Y] = "pY";
    roles[Qt::UserRole + (int)Z] = "pZ";
    return roles;
}
