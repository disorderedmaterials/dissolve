// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/masterTermModel.h"

MasterTermModel::MasterTermModel(QObject *parent) : QAbstractTableModel(parent) {}

void MasterTermModel::setIconFunction(std::function<bool(std::string_view termName)> func) { iconFunction_ = std::move(func); }

int MasterTermModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 3; }

Qt::ItemFlags MasterTermModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant MasterTermModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (MasterTermModelData::DataType::Name):
                return "Name";
            case (MasterTermModelData::DataType::Form):
                return "Form";
            case (MasterTermModelData::DataType::Parameters):
                return "Parameters";
            case (MasterTermModelData::DataType::Electrostatic14Scale):
                return "Elec 1-4";
            case (MasterTermModelData::DataType::VanDerWaals14Scale):
                return "vdW 1-4";
            default:
                return {};
        }

    return {};
}

QVariant MasterTermModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= rowCount())
        return {};

    if (role == MasterTermModelData::Roles::HasMaster && iconFunction_)
        return iconFunction_(getTermData(index.row(), MasterTermModelData::DataType::Name).toString().toStdString());

    if (role == MasterTermModelData::Roles::Icon && iconFunction_)
        return QIcon(iconFunction_(getTermData(index.row(), MasterTermModelData::DataType::Name).toString().toStdString())
                         ? ":/general/icons/warn.svg"
                         : ":/general/icons/warn.svg");

    if (role == MasterTermModelData::Roles::Display || role == MasterTermModelData::Roles::Edit)
        return getTermData(index.row(), static_cast<MasterTermModelData::DataType>(index.column()));

    return {};
}

bool MasterTermModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if (!setTermData(index.row(), static_cast<MasterTermModelData::DataType>(index.column()), value))
        return false;

    emit dataChanged(index, index);

    return true;
}

QHash<int, QByteArray> MasterTermModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::DecorationRole] = "icon";
    return roles;
}
