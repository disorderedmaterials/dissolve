// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/cifAssemblyModel.h"
#include "classes/empiricalFormula.h"

CIFAssemblyModel::CIFAssemblyModel(std::vector<CIFAssembly> &assemblies) : assemblies_(assemblies) {}

int CIFAssemblyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return assemblies_.size();
    return assemblies_[parent.row()].nGroups();
}

int CIFAssemblyModel::columnCount(const QModelIndex &parent) const { return 3; }

QModelIndex CIFAssemblyModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

bool CIFAssemblyModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QVariant CIFAssemblyModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::UserRole && role != Qt::EditRole && role != Qt::CheckStateRole)
        return QVariant();

    // Assembly (root item)
    if (!index.parent().isValid())
    {
        if (index.row() > assemblies_.size())
            return QVariant();
        if (index.column() > 0)
            return QVariant();
        auto &a = assemblies_[index.row()];
        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                return QString::fromStdString(std::string(a.name()));
            case Qt::UserRole:
                return QVariant::fromValue(a);
            default:
                return QVariant();
        }
    }

    // Group (column 1)
    auto &assembly = assemblies_[index.parent().row()];
    auto &groups = assembly.groups();
    if (index.row() > groups.size())
        return QVariant();
    if (index.column() > 2)
        return QVariant();
    auto &g = groups[index.row()];
    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (index.column())
            {
                case 1:
                    return QString::fromStdString(std::string(g.name()));
                case 2:
                    return QString::fromStdString(EmpiricalFormula::formula(g.atoms(), [](const auto &i) { return i.Z(); }));
                default:
                    return QVariant();
            }
        case Qt::CheckStateRole:
            if (index.column() == 1)
                return g.active() ? Qt::Checked : Qt::Unchecked;
            else
                return QVariant();
        case Qt::UserRole:
            switch (index.column())
            {
                case 1:
                    return QVariant::fromValue(g);
                default:
                    return QVariant();
            }
        default:
            return QVariant();
    }
}

QModelIndex CIFAssemblyModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = parent.row() + 1;
    else
        return QModelIndex();

    return createIndex(row, column, child);
}

QVariant CIFAssemblyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "Assembly";
        case 1:
            return "Group";
        case 2:
            return "Chemical Formula";
        default:
            return QVariant();
    }
}

Qt::ItemFlags CIFAssemblyModel::flags(const QModelIndex &index) const
{
    // Assembly (root item)
    if (!index.parent().isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else if (index.column() == 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool CIFAssemblyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Only editable data is the check state of groups (column 1)
    if (!index.parent().isValid() || index.column() != 1 || role != Qt::CheckStateRole)
        return false;

    // Get assembly and check for number of groups > 1
    auto &assembly = assemblies_[index.parent().row()];

    // Get group being modified and set the active flag
    auto &group = assembly.groups()[index.row()];
    group.setActive(value.value<Qt::CheckState>() == Qt::Checked);

    Q_EMIT(dataChanged(index, index));

    return true;
}
