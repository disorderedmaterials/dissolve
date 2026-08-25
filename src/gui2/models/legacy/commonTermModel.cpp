// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/commonTermModel.h"

CommonTermModel::CommonTermModel(Species *species) : species_(species) {}

void CommonTermModel::setQueryFunction(std::function<bool(std::string_view termName, Species *sp)> func)
{
    queryFunction_ = std::move(func);
}

int CommonTermModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 3; }

Qt::ItemFlags CommonTermModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant CommonTermModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (CommonTermModelData::DataType::Name):
                return "Name";
            case (CommonTermModelData::DataType::Form):
                return "Form";
            case (CommonTermModelData::DataType::Parameters):
                return "Parameters";
            case (CommonTermModelData::DataType::Electrostatic14Scale):
                return "Elec 1-4";
            case (CommonTermModelData::DataType::VanDerWaals14Scale):
                return "vdW 1-4";
            default:
                return {};
        }

    return {};
}

QVariant CommonTermModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= rowCount())
        return {};

    if (role == CommonTermModelData::Roles::Query && queryFunction_)
        return species_ &&
               queryFunction_(getTermData(index.row(), CommonTermModelData::DataType::Name).toString().toStdString(), species_);

    if (role == Qt::DecorationRole && queryFunction_)
        return QIcon(
            queryFunction_(getTermData(index.row(), CommonTermModelData::DataType::Name).toString().toStdString(), species_)
                ? ":/general/icons/warn.svg"
                : ":/general/icons/true.svg");

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return getTermData(index.row(), static_cast<CommonTermModelData::DataType>(index.column()));

    return {};
}

bool CommonTermModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if (!setTermData(index.row(), static_cast<CommonTermModelData::DataType>(index.column()), value))
        return false;

    Q_EMIT dataChanged(index, index);

    return true;
}

QHash<int, QByteArray> CommonTermModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[CommonTermModelData::Query] = "query";
    return roles;
}
