// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/xmlBondModel.h"

XmlBondModel::XmlBondModel() {}

void XmlBondModel::readFile(const pugi::xml_node &root)
{
    beginResetModel();
    bonds_.clear();

    for (auto &b : root.select_nodes("/ForceField/HarmonicBondForce/Bond"))
    {
        bonds_.emplace_back(b.node().attribute("class1").as_string(), b.node().attribute("class2").as_string(),
                            // Convert from nm to Å
                            b.node().attribute("length").as_double() * 10.0,
                            // Convert from kJ/mol/nm² to kJ/mol/Å²
                            b.node().attribute("k").as_double() / 100.0);
    }
    endResetModel();
}

int XmlBondModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return bonds_.size();
}

int XmlBondModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant XmlBondModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        switch (index.column())
        {
            case 0:
                return "AtomI";
            case 1:
                return "AtomJ";
            case 2:
                return "Length";
            case 3:
                return "k";
            default:
                return QVariant();
        }

    if (role != Qt::DisplayRole)
        return QVariant();
    switch (index.column())
    {
        case 0:
            return QVariant(QString(std::get<0>(bonds_[index.row()]).c_str()));
        case 1:
            return QVariant(QString(std::get<1>(bonds_[index.row()]).c_str()));
        case 2:
            return QVariant(std::get<2>(bonds_[index.row()]));
        case 3:
            return QVariant(std::get<3>(bonds_[index.row()]));
        default:
            return QVariant();
    }
}

QVariant XmlBondModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "AtomI";
        case 1:
            return "AtomJ";
        case 2:
            return "Length";
        case 3:
            return "k";
        default:
            return QVariant();
    }
}

std::vector<ForcefieldBondTerm> XmlBondModel::toVector()
{
    std::vector<ForcefieldBondTerm> result;
    for (auto &bond : bonds_)
        result.emplace_back(std::get<0>(bond), std::get<1>(bond), SpeciesBond::HarmonicForm,
                            std::vector({std::get<3>(bond), std::get<2>(bond)}));
    return result;
}

void XmlBondModel::clear()
{
    beginResetModel();
    bonds_.clear();
    endResetModel();
}
