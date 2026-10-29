// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/plot/plot1DModel.h"

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
    roles[Qt::UserRole + (int)X] = "x";
    roles[Qt::UserRole + (int)Y] = "y";
    return roles;
}

//
QString Plot1DModel::name() { return QString::fromStdString(name_); }
