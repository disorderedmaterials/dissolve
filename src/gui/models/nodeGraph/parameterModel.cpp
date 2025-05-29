// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "parameterModel.h"
#include "nodes/number.h"
#include <qvariant.h>

enum Roles
{
    NAME = 0,
    DESCRIPTION,
    TYPE,
    DATA,
};

ParameterModel::ParameterModel(Node::NodeParameterMap &values) : values_(values) {}

// Return number of parameters (required by QAbstractListModel)
int ParameterModel::rowCount(const QModelIndex &parent) const { return values_.size(); }

// Access edge by QModelIndex.  The correct role can be found in the roleNames function.
QVariant ParameterModel::data(const QModelIndex &index, int role) const
{
    auto it = std::next(values_.begin(), index.row());
    switch (role - Qt::UserRole)
    {
        case NAME:
            return QString::fromStdString(it->first);
        case DESCRIPTION:
            return QString::fromStdString(std::string(it->second->description()));
        case DATA:
            if (it->second->type() == typeid(Number))
                return QVariant::fromValue(it->second->upcast<Number>()->get().asInteger());
            if (it->second->type() == typeid(bool))
                return QVariant::fromValue(it->second->upcast<bool>()->get());
            return QString::fromStdString("Unrepresentable");
        case TYPE:
            if (it->second->type() == typeid(Number))
                return "number";
            if (it->second->type() == typeid(bool))
                return "bool";
            return "unknown";

        default:
            return {};
    }
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> ParameterModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[Qt::UserRole + (int)NAME] = "name";
    result[Qt::UserRole + (int)DESCRIPTION] = "description";
    result[Qt::UserRole + (int)DATA] = "param";
    result[Qt::UserRole + (int)TYPE] = "type";
    return result;
}
