// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "parameterModel.h"
#include <qvariant.h>

ParameterModel::ParameterModel(std::map<std::string_view, std::shared_ptr<ParameterBase>> &values) : values_(values) {}

int ParameterModel::rowCount(const QModelIndex &parent) const { return values_.size(); }

QVariant ParameterModel::data(const QModelIndex &index, int role) const
{
    auto it = std::next(values_.begin(), index.row());
    switch (role - Qt::UserRole)
    {
        case 0:
            return QString::fromStdString(std::string(it->first));
        case 1:
            return QString::fromStdString(std::string(it->second->description()));
        default:
            return {};
    }
}

QHash<int, QByteArray> ParameterModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[Qt::UserRole] = "name";
    result[Qt::UserRole + 1] = "description";
    return result;
}
