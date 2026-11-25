// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plot2DModel.h"
#include <QPoint>
#include <algorithm>
#include <cmath>

Plot2DModel::Plot2DModel(const std::string &name, const Data2D *sourceData)
    : PlotModelBase(2), name_(name), sourceData_(sourceData)
{
    generateIndices(sourceData_->xAxis().size(), sourceData_->yAxis().size());
}

// Return the name of the dataset being plotted
QString Plot2DModel::name() { return QString::fromStdString(name_); }

// Return the minimum x-value in the dataset
double Plot2DModel::minX()
{
    const auto &x = sourceData_->xAxis();
    if (x.empty())
        return 0;
    return *std::min_element(x.begin(), x.end());
}

// Return the minimum y-value in the dataset
double Plot2DModel::minY()
{
    const auto &y = sourceData_->yAxis();
    if (y.empty())
        return 0;
    return *std::min_element(y.begin(), y.end());
}

// Return the minimum z-value in the dataset
double Plot2DModel::minZ() { return sourceData_->minValue(); }

// Return the maximum x-value in the dataset
double Plot2DModel::maxX()
{
    const auto &x = sourceData_->xAxis();
    if (x.empty())
        return 0;
    auto max = *std::max_element(x.cbegin(), x.cend());
    return max;
}

// Return the maximum y-value in the dataset
double Plot2DModel::maxY()
{
    const auto &y = sourceData_->yAxis();
    if (y.empty())
        return 0;
    auto max = *std::max_element(y.cbegin(), y.cend());
    return max;
}

// Return the maximum z-value in the dataset
double Plot2DModel::maxZ() { return sourceData_->maxValue(); }

// Return the name of the x-axis
QString Plot2DModel::xName() { return QString::fromStdString(xName_); }

// Return the name of the y-axis
QString Plot2DModel::yName() { return QString::fromStdString(yName_); }

// Return the name of the z-axis
QString Plot2DModel::zName() { return QString::fromStdString(zName_); }

// Return the current number of x-axis values
int Plot2DModel::currentNXValues() { return currentNXValues_; }

// Return the current number of y-axis values
int Plot2DModel::currentNYValues() { return currentNYValues_; }

// Return the source data pointer
const Data2D *Plot2DModel::sourceData() const { return sourceData_; }

// Perform any required updates to the model when the data is changed
void Plot2DModel::handleDataChanged()
{
    const auto &source = sourceData_;
    const auto nX = source->xAxis().size();
    const auto nY = source->yAxis().size();
    if (nX != currentNXValues_ || nY != currentNYValues_)
        generateIndices(nX, nY);
    for (int i = 0; i < indices_.size(); i++)
    {
        auto index = this->index(i, 0);
        Q_EMIT dataChanged(index, index, {Qt::UserRole + INDEX});
    }
    Q_EMIT contentChanged();
}

// Generate the indices for the x-y plane, given the numbe rof x- and y-axis values
void Plot2DModel::generateIndices(int nX, int nY)
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

// Returns the z value at a given x-y point
double Plot2DModel::z(int x, int y) { return sourceData_->value(x, y); }

/*
 * QAbstractTableModel overrides
 */

// Number of rows
int Plot2DModel::rowCount(const QModelIndex &parent) const { return sourceData_->nValues(); }

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
