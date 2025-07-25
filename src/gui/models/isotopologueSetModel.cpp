// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/isotopologueSetModel.h"
#include "classes/species.h"

IsotopologueSetModel::IsotopologueSetModel(OptionalReferenceWrapper<IsotopologueSet> set) : set_(set) {}

void IsotopologueSetModel::setSourceData(IsotopologueSet &set)
{
    beginResetModel();
    set_ = set;
    endResetModel();
}

std::vector<QModelIndex> IsotopologueSetModel::addMissingSpecies(const std::vector<std::unique_ptr<Species>> &species)
{
    if (!set_)
        return {};

    auto &set = set_->get();

    auto nAdded = 0;
    for (const auto &sp : species)
    {
        if (!set.contains(sp.get()))
        {
            beginInsertRows({}, set.nSpecies(), set.nSpecies());
            set.add(sp->naturalIsotopologue(), 1.0);
            endInsertRows();
            ++nAdded;
        }
    }

    std::vector<QModelIndex> newIndices;
    for (auto n = set.nSpecies() - nAdded; n < set.nSpecies(); ++n)
        newIndices.push_back(createIndex(n, 0));
    if (!newIndices.empty())
        Q_EMIT(dataChanged(createIndex(set.nSpecies() - nAdded, 0), createIndex(set.nSpecies() - 1, 0)));
    return newIndices;
}

void IsotopologueSetModel::addIsotopologueWeight(const QModelIndex index)
{
    // Check index validity
    if (!set_ || !index.isValid())
        return;

    auto &set = set_->get();
    const auto topeIndex = index.parent().isValid() ? index.parent().row() : index.row();
    auto &topes = set.isotopologues()[topeIndex];
    const auto *sp = topes.species();

    if (!topes.mix().contains(sp->naturalIsotopologue()))
    {
        beginInsertRows(createIndex(topeIndex, 0), topes.mix().size(), topes.mix().size());
        set.add(sp->naturalIsotopologue(), 1.0);
        endInsertRows();
    }
    else
    {
        for (auto &tope : sp->isotopologues())
        {
            if (!topes.mix().contains(tope.get()))
            {
                beginInsertRows(createIndex(topeIndex, 0), topes.mix().size(), topes.mix().size());
                set.add(tope.get(), 1.0);
                endInsertRows();
                break;
            }
        }
    }

    Q_EMIT(dataChanged(index, index));
}

void IsotopologueSetModel::removeIndex(const QModelIndex index)
{
    // Check index validity
    if (!set_ || !index.isValid())
        return;

    auto &set = set_->get();

    if (!index.parent().isValid())
    {
        // Root item (Isotopologues)
        auto &topes = set.isotopologues()[index.row()];
        beginRemoveRows({}, index.row(), index.row());
        set.remove(topes.species());
        endRemoveRows();
    }
    else
    {
        // Secondary item (IsotopologueWeight)
        auto &topes = set.isotopologues()[index.parent().row()];
        auto mixItem = topes.mix().key(index.row());
        beginRemoveRows(index.parent(), index.row(), index.row());
        topes.mix().erase(mixItem);
        endRemoveRows();
    }

    Q_EMIT(dataChanged(index, index));
}

/*
 * QAbstractItemModel overrides
 */

int IsotopologueSetModel::rowCount(const QModelIndex &parent) const
{
    if (!set_)
        return 0;

    if (!parent.isValid())
        return set_->get().isotopologues().size();
    return set_->get().isotopologues()[parent.row()].mix().size();
}

int IsotopologueSetModel::columnCount(const QModelIndex &parent) const { return 3; }

QModelIndex IsotopologueSetModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return {};
    return createIndex(index.internalId() - 1, 0, root);
}

bool IsotopologueSetModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QVariant IsotopologueSetModel::data(const QModelIndex &index, int role) const
{
    if (!set_ || (role != Qt::DisplayRole && role != Qt::UserRole && role != Qt::EditRole))
        return {};

    const auto &set = set_->get();

    // Root item (IsotopologueSet)
    if (!index.parent().isValid())
    {
        if (index.row() > set.isotopologues().size())
            return {};

        const auto &tope = set.isotopologues()[index.row()];

        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                if (index.column() == 0)
                    return QString::fromStdString(std::string(tope.species()->name()));
                else
                    return {};
            case Qt::UserRole:
                return QVariant::fromValue(&tope);
            default:
                return {};
        }
    }

    // Secondary item (Isotopologue/Weight)
    const auto &tope = set.isotopologues()[index.parent().row()];
    const auto &[iso, weight] = tope.mix().pair(index.row());
    if (!index.parent().parent().isValid())
    {
        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                if (index.column() == 1)
                    return QString::fromStdString(std::string(iso->name()));
                else if (index.column() == 2)
                    return weight;
                break;
            case Qt::UserRole:
                return QVariant::fromValue(iso);
            default:
                return {};
        }
    }

    return {};
}

QModelIndex IsotopologueSetModel::index(int row, int column, const QModelIndex &parent) const
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

QVariant IsotopologueSetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    switch (section)
    {
        case 0:
            return "Species";
        case 1:
            return "Isotopologue";
        case 2:
            return "Weight";
        default:
            return {};
    }
}

Qt::ItemFlags IsotopologueSetModel::flags(const QModelIndex &index) const
{
    if (!index.parent().isValid() || index.column() == 0)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool IsotopologueSetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Only editable items are the children of the root items
    if (!index.parent().isValid() || !set_)
        return false;

    // Check row index
    auto &set = set_->get();
    if (index.parent().row() > set.isotopologues().size())
        return false;

    auto &tope = set.isotopologues()[index.parent().row()];
    auto &[iso, weight] = tope.mix().pair(index.row());

    if (index.column() == 1)
    {
        // Convert value to Isotopologue for species
        const auto *namedIso = tope.species()->findIsotopologue(value.toString().toStdString());
        if (!namedIso || namedIso == iso)
            return false;
        tope.mix().changeKey(iso, namedIso);
        Q_EMIT(dataChanged(index, index));
        return true;
    }
    else if (index.column() == 2)
    {
        const auto newWeight = value.toDouble();
        if (newWeight < 0.0)
            return false;
        weight = newWeight;
        Q_EMIT(dataChanged(index, index));
        return true;
    }

    return false;
}
