// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/masterTermTreeModel.h"

MasterTermModel &MasterTermTreeModel::modelForTopLevelRow(int row)
{
    switch (row)
    {
        case (0):
            return bondModel_;
        case (1):
            return angleModel_;
        case (2):
            return torsionModel_;
        case (3):
            return improperModel_;
        default:
            throw(std::runtime_error("Invalid row provided, so can't return top level model.\n"));
    }
}

void MasterTermTreeModel::setData(std::vector<std::shared_ptr<MasterBond>> &bonds,
                                  std::vector<std::shared_ptr<MasterAngle>> &angles,
                                  std::vector<std::shared_ptr<MasterTorsion>> &torsions,
                                  std::vector<std::shared_ptr<MasterImproper>> &impropers)
{
    beginResetModel();
    bondModel_.setSourceData(bonds);
    angleModel_.setSourceData(angles);
    torsionModel_.setSourceData(torsions);
    improperModel_.setSourceData(impropers);
    endResetModel();
}

void MasterTermTreeModel::setBondIconFunction(std::function<bool(std::string_view termName)> func)
{
    bondModel_.setIconFunction(func);
}

void MasterTermTreeModel::setAngleIconFunction(std::function<bool(std::string_view termName)> func)
{
    angleModel_.setIconFunction(func);
}

void MasterTermTreeModel::setTorsionIconFunction(std::function<bool(std::string_view termName)> func)
{
    torsionModel_.setIconFunction(func);
}

void MasterTermTreeModel::setImproperIconFunction(std::function<bool(std::string_view termName)> func)
{
    improperModel_.setIconFunction(func);
}

void MasterTermTreeModel::prefixNames(QList<QModelIndex> indices, QString prefix)
{
    for (auto &index : indices)
    {
        // Ignore root items
        if (!index.parent().isValid() || index.column() == 0)
            continue;
        auto &rootModel = modelForTopLevelRow(index.parent().row());
        rootModel.setTermData(index.row(), MasterTermModelData::DataType::Name,
                              prefix + rootModel.getTermData(index.row(), MasterTermModelData::DataType::Name).toString());
    }
}

void MasterTermTreeModel::suffixNames(QList<QModelIndex> indices, QString suffix)
{
    for (auto &index : indices)
    {
        // Ignore root items
        if (!index.parent().isValid() || index.column() == 0)
            continue;
        auto &rootModel = modelForTopLevelRow(index.parent().row());
        rootModel.setTermData(index.row(), MasterTermModelData::DataType::Name,
                              rootModel.getTermData(index.row(), MasterTermModelData::DataType::Name).toString() + suffix);
    }
}

int MasterTermTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        switch (parent.row())
        {
            case (0):
                return bondModel_.rowCount();
            case (1):
                return angleModel_.rowCount();
            case (2):
                return torsionModel_.rowCount();
            case (3):
                return improperModel_.rowCount();
            default:
                return 0;
        }
    }
    else
        return 4;
}

int MasterTermTreeModel::columnCount(const QModelIndex &parent) const { return 2; }

Qt::ItemFlags MasterTermTreeModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool MasterTermTreeModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QModelIndex MasterTermTreeModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

QModelIndex MasterTermTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = parent.row() + 1;
    else
        return {};

    return createIndex(row, column, child);
}

QVariant MasterTermTreeModel::headerData(int column, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (column)
    {
        case (0):
            return "Type";
        case (1):
            return "Name";
        default:
            return {};
    }
}

QVariant MasterTermTreeModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole)
        return {};

    if (index.parent().isValid())
    {
        if (index.column() != 1)
            return {};
        switch (index.parent().row())
        {
            case (0):
                if (role == Qt::DecorationRole)
                    return bondModel_.data(index, Qt::DecorationRole);
                else
                    return bondModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            case (1):
                if (role == Qt::DecorationRole)
                    return angleModel_.data(index, Qt::DecorationRole);
                else
                    return angleModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            case (2):
                if (role == Qt::DecorationRole)
                    return torsionModel_.data(index, Qt::DecorationRole);
                else
                    return torsionModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            case (3):
                if (role == Qt::DecorationRole)
                    return improperModel_.data(index, Qt::DecorationRole);
                else
                    return improperModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            default:
                return {};
        }
    }
    else if (index.column() == 0 && role != Qt::DecorationRole)
        switch (index.row())
        {
            case (0):
                return "Bonds";
            case (1):
                return "Angles";
            case (2):
                return "Torsions";
            case (3):
                return "Impropers";
            default:
                return {};
        }

    return {};
}

bool MasterTermTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || !index.parent().isValid())
        return false;

    switch (index.parent().row())
    {
        case (0):
            if (!bondModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        case (1):
            if (!angleModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        case (2):
            if (torsionModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        case (3):
            if (improperModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        default:
            return false;
    }

    Q_EMIT dataChanged(index, index);
    return true;
}
