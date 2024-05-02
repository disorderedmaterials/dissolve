// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/speciesIsoModel.h"
#include "classes/atomType.h"
#include "gui/delegates/isotopeCombo.h"

SpeciesIsoModel::SpeciesIsoModel(Species &species) : species_(species) {}

int SpeciesIsoModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return species_.isotopologues().size();
    return species_.isotopologue(parent.row())->isotopes().size();
}

int SpeciesIsoModel::columnCount(const QModelIndex &parent) const { return 3; }

QModelIndex SpeciesIsoModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return {};
    return createIndex(index.internalId() - 1, 0, root);
}

bool SpeciesIsoModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QVariant SpeciesIsoModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::UserRole && role != Qt::EditRole)
        return {};

    if (!index.parent().isValid())
    {
        if (index.row() > species_.isotopologues().size())
            return {};
        auto iso = species_.isotopologue(index.row());
        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                if (index.column() > 0)
                    return {};
                return QString::fromStdString(std::string(iso->name()));
            case Qt::UserRole:
                return QVariant::fromValue(iso);
            default:
                return {};
        }
    }

    auto [atomType, isotope] = species_.isotopologue(index.parent().row())->isotopes()[index.row()];
    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (index.column())
            {
                case 1:
                    return QString::fromStdString(std::string(atomType->name()));
                case 2:
                    if (Sears91::A(isotope) == 0)
                        return QString("Natural (bc = %1 fm)").arg(Sears91::boundCoherent(isotope));
                    else
                        return QString("%1 (bc = %2 fm)").arg(Sears91::A(isotope)).arg(Sears91::boundCoherent(isotope));
                default:
                    return {};
            }
        case Qt::UserRole:
            switch (index.column())
            {
                case 1:
                    return QVariant::fromValue(atomType);
                case 2:
                    return QVariant::fromValue(isotope);
                default:
                    return {};
            }
        default:
            return {};
    }
}

QModelIndex SpeciesIsoModel::index(int row, int column, const QModelIndex &parent) const
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

QVariant SpeciesIsoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};
    switch (section)
    {
        case 0:
            return "Isotopologue";
        case 1:
            return "AtomType";
        case 2:
            return "Isotope";
        default:
            return {};
    }
}

Qt::ItemFlags SpeciesIsoModel::flags(const QModelIndex &index) const
{
    if (!index.parent().isValid())
    {
        if (index.column() == 0)
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    if (index.column() == 2)
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool SpeciesIsoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole && role != Qt::UserRole)
        return false;

    // Root Isotopologue?
    if (!index.parent().isValid())
    {
        auto iso = species_.isotopologue(index.row());
        iso->setName(DissolveSys::uniqueName(DissolveSys::niceName(value.toString().toStdString()), species_.isotopologues(),
                                             [&](const auto &i) { return iso == i.get() ? "" : i->name(); }));
        Q_EMIT(dataChanged(index, index));
        return true;
    }

    // Isotope data
    if (index.column() != 2)
        return false;

    auto *isotopologue = species_.isotopologue(index.parent().row());
    auto [atomType, isotope] = isotopologue->isotopes()[index.row()];
    auto newIso = value.value<Sears91::Isotope>();

    // Check that requested isotope is compatible with the atomtype's element
    if (Sears91::Z(newIso) != atomType->Z())
        return false;

    // All good, so set the data
    isotopologue->setAtomTypeIsotope(atomType, newIso);
    Q_EMIT(dataChanged(index, index));
    return true;
}

QModelIndex SpeciesIsoModel::addIso()
{
    beginInsertRows({}, species_.nIsotopologues(), species_.nIsotopologues());
    species_.addIsotopologue("NewIsotopologue");
    endInsertRows();

    auto newIndex = createIndex(species_.nIsotopologues() - 1, 0);
    Q_EMIT(dataChanged(newIndex, newIndex));
    return newIndex;
}

void SpeciesIsoModel::removeIso(const QModelIndex &index)
{
    // We might be given a child of the isotopologue...
    auto row = index.parent().isValid() ? index.parent().row() : index.row();
    beginRemoveRows({}, row, row);
    species_.removeIsotopologue(species_.isotopologue(row));
    endRemoveRows();
}
