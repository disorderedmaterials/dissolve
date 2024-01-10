// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/enumOptionsModel.h"
#include "base/enumOptionsBase.h"

// Set source AtomType data
void EnumOptionsModel::setData(const EnumOptionsBase &options)
{
    beginResetModel();
    enumOptions_ = options;
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int EnumOptionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return enumOptions_ ? enumOptions_->get().nOptions() : 0;
}

QVariant EnumOptionsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || index.column() != 0)
        return {};

    return QString::fromStdString(enumOptions_->get().keywordByIndex(index.row()));
}

Qt::ItemFlags EnumOptionsModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant EnumOptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (section == 0)
        return "Option";

    return {};
}
