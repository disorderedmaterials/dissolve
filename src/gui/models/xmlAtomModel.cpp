// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/xmlAtomModel.h"
#include "classes/atomtype.h"
#include "data/elements.h"
#include <QColor>
#include <pugixml.hpp>

XmlAtomModel::XmlAtomModel(Dissolve &dissolve) : dissolve_(dissolve) {}

void XmlAtomModel::readFile(const pugi::xml_node &root)
{
    beginResetModel();
    atoms_.clear();

    auto types = dissolve_.atomTypes();

    for (auto &b : root.select_nodes("/ForceField/AtomTypes/Type"))
    {
        auto nonbonded = root.select_node(
            fmt::format("/ForceField/NonbondedForce/Atom[@type = '{}']", b.node().attribute("name").as_string()).c_str());

        atoms_.emplace_back(b.node().attribute("name").as_string(), b.node().attribute("class").as_string(),
                            b.node().attribute("element").as_string(), b.node().attribute("mass").as_double(),
                            nonbonded.node().attribute("charge").as_double(),
                            // Convert sigma from nm to Å
                            nonbonded.node().attribute("sigma").as_double() * 10,
                            nonbonded.node().attribute("epsilon").as_double());
    }

    endResetModel();
}

int XmlAtomModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return atoms_.size();
}

int XmlAtomModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QVariant XmlAtomModel::data(const QModelIndex &index, int role) const
{
    int type;

    if (role == Qt::ToolTipRole)
        switch (index.column())
        {
            case 0:
                return "Name";
            case 1:
                return "Class";
            case 2:
                return "Element";
            case 3:
                return "Mass";
            case 4:
                return "Charge";
            case 5:
                return "Sigma";
            case 6:
                return "Epsilon";
            default:
                return QVariant();
        }

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
        case 0:
            return std::get<0>(atoms_[index.row()]).c_str();
        case 1:
            return std::get<1>(atoms_[index.row()]).c_str();
        case 2:
            return std::get<2>(atoms_[index.row()]).c_str();
        case 3:
            return std::get<3>(atoms_[index.row()]);
        case 4:
            return std::get<4>(atoms_[index.row()]);
        case 5:
            return std::get<5>(atoms_[index.row()]);
        case 6:
            return std::get<6>(atoms_[index.row()]);
        default:
            return QVariant();
    }
}

QVariant XmlAtomModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "Name";
        case 1:
            return "Class";
        case 2:
            return "Element";
        case 3:
            return "Mass";
        case 4:
            return "Charge";
        case 5:
            return "Sigma";
        case 6:
            return "Epsilon";
        default:
            return QVariant();
    }
}

void XmlAtomModel::clear()
{
    beginResetModel();
    atoms_.clear();
    endResetModel();
}

std::vector<ForcefieldAtomType> XmlAtomModel::toVector()
{
    std::vector<ForcefieldAtomType> result;
    int index = 0;
    for (auto &at : atoms_)
    {
        std::vector<double> params = {std::get<6>(at), std::get<5>(at)};
        result.emplace_back(Elements::element(std::get<2>(at)), index++, std::get<1>(at), "", "", std::get<4>(at), params);
    }
    return result;
}
