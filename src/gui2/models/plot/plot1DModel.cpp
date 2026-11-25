// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plot1DModel.h"
#include <algorithm>
#include <cmath>

Plot1DModel::Plot1DModel(const std::string &name, const Data1D *sourceData)
    : PlotModelBase(1), name_(name), sourceData_(sourceData)
{
}

//
QString Plot1DModel::name() { return QString::fromStdString(name_); }

//
double Plot1DModel::minX()
{
    const auto &x = sourceData_->xAxis();
    if (x.empty())
        return 0;
    return *std::min_element(x.begin(), x.end());
}

//
double Plot1DModel::minY() { return sourceData_->minValue(); }

//
double Plot1DModel::maxX()
{
    const auto &x = sourceData_->xAxis();
    if (x.empty())
        return 0;
    auto max = *std::max_element(x.cbegin(), x.cend());
    return max;
}

//
double Plot1DModel::maxY() { return sourceData_->maxValue(); }

//
QString Plot1DModel::xName() { return QString::fromStdString(xName_); }

//
QString Plot1DModel::yName() { return QString::fromStdString(yName_); }

//
void Plot1DModel::handleDataChanged()
{
    const auto &source = sourceData_;
    const auto n = source->nValues();
    for (int i = 0; i < n; i++)
    {
        auto index = this->index(i, 0);
        Q_EMIT dataChanged(index, index, {Qt::UserRole + X});
        Q_EMIT dataChanged(index, index, {Qt::UserRole + Y});
    }
    Q_EMIT contentChanged();
}

/*
 * QAbstractTableModel overrides
 */

// Number of rows
int Plot1DModel::rowCount(const QModelIndex &parent) const { return sourceData_->nValues(); }

// Access edge by QModelIndex.  The correct role can be found in the roleNames function.
QVariant Plot1DModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= rowCount())
        return {};

    switch (role - Qt::UserRole)
    {
        case X:
            return sourceData_->xAxis()[row];
        case Y:
            return sourceData_->values()[row];
    }
    return {};
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> Plot1DModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)X] = "pX";
    roles[Qt::UserRole + (int)Y] = "pY";
    return roles;
}
