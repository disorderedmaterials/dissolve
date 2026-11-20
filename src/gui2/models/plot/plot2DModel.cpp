// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plot2DModel.h"
#include <QPoint>
#include <algorithm>
#include <cmath>

Plot2DModel::Plot2DModel(const std::string &name, const std::function<const Data2D *()> &sourceDataGetter)
    : PlotModelBase(2), name_(name), sourceDataGetter_(sourceDataGetter)
{
    regenerateIndices(sourceDataGetter_()->xAxis().size(), sourceDataGetter_()->yAxis().size());
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
int Plot2DModel::currentNXValues() { return currentNXValues_; }

//
int Plot2DModel::currentNYValues() { return currentNYValues_; }

//
void Plot2DModel::handleDataChanged()
{
    const auto &source = sourceDataGetter_();
    const auto nX = source->xAxis().size();
    const auto nY = source->yAxis().size();
    if (nX != currentNXValues_ || nY != currentNYValues_)
        regenerateIndices(nX, nY);
    for (int i = 0; i < indices_.size(); i++)
    {
        auto index = this->index(i, 0);
        Q_EMIT dataChanged(index, index, {Qt::UserRole + INDEX});
    }
    Q_EMIT contentChanged();
}

//
void Plot2DModel::regenerateIndices(int nX, int nY)
{
    const int nIndices = nX * nY;
    indices_.clear();
    indices_.reserve(nIndices);
    for (int i = 0; i < nX; i++)
        for (int j = 0; j < nY; j++)
            indices_.emplace_back(i, j);
    currentNXValues_ = nX;
    currentNYValues_ = nY;
}

//
double Plot2DModel::z(int x, int y) { return sourceDataGetter_()->value(x, y); }

/*
 * QAbstractTableModel overrides
 */

// Number of rows
int Plot2DModel::rowCount(const QModelIndex &parent) const { return sourceDataGetter_()->nValues(); }

// Access edge by QModelIndex.  The correct role can be found in the roleNames function.
QVariant Plot2DModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();

    if (row >= rowCount())
        return {};

    if (role - Qt::UserRole != INDEX)
        return {};

    const auto &point = indices_[row];
    return QPoint(point.first, point.second);
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> Plot2DModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)INDEX] = "pXY";
    return roles;
}
