// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plot1DModel.h"
#include <algorithm>
#include <cmath>

Plot1DModel::Plot1DModel(const std::string &name, const std::function<const Data1D *()> &sourceDataGetter)
    : name_(name), sourceDataGetter_(sourceDataGetter)
{
}

//
void Plot1DModel::reset()
{
    beginResetModel();
    endResetModel();
}

// Number of rows
int Plot1DModel::rowCount(const QModelIndex &parent) const { return sourceDataGetter_()->nValues(); }

// Access edge by QModelIndex.  The correct role can be found in the roleNames function.
QVariant Plot1DModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= rowCount())
        return {};

    switch (role - Qt::UserRole)
    {
        case X:
            return sourceDataGetter_()->xAxis()[row];
        case Y:
            return sourceDataGetter_()->values()[row];
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

//
QString Plot1DModel::name() { return QString::fromStdString(name_); }

//
double Plot1DModel::minX()
{
    const auto &x = sourceDataGetter_()->xAxis();
    if (x.empty())
        return 0;
    return *std::min_element(x.begin(), x.end());
}

//
double Plot1DModel::minY() { return sourceDataGetter_()->minValue(); }

//
double Plot1DModel::maxX()
{
    const auto &x = sourceDataGetter_()->xAxis();
    if (x.empty())
        return 0;
    auto max = *std::max_element(x.cbegin(), x.cend());
    return max;
}

//
double Plot1DModel::maxY() { return sourceDataGetter_()->maxValue(); }

//
QString Plot1DModel::xName() { return QString::fromStdString(xName_); }

//
QString Plot1DModel::yName() { return QString::fromStdString(yName_); }

//
double Plot1DModel::xOffsetPercentage()
{
    return 0;
}

//
double Plot1DModel::yOffsetPercentage()
{
    return 0;
}