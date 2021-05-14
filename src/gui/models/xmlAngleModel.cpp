// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/xmlAngleModel.h"
#include <pugixml.hpp>

XmlAngleModel::XmlAngleModel() {}

void XmlAngleModel::readFile(const pugi::xml_node &root)
{
    beginResetModel();
    angles_.clear();

    for (auto &a : root.select_nodes("/ForceField/HarmonicAngleForce/Angle"))
    {
        angles_.emplace_back(a.node().attribute("class1").as_string(), a.node().attribute("class2").as_string(),
                             a.node().attribute("class3").as_string(), a.node().attribute("angle").as_double() * 180 / PI,
                             a.node().attribute("k").as_double());
    }

    endResetModel();
}

int XmlAngleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return angles_.size();
}

int XmlAngleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant XmlAngleModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        switch (index.column())
        {
            case 0:
                return "AtomI";
            case 1:
                return "AtomJ";
            case 2:
                return "AtomK";
            case 3:
                return "Angle";
            case 4:
                return "k";
            default:
                return QVariant();
        }

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
        case 0:
            return QVariant(QString(std::get<0>(angles_[index.row()]).c_str()));
        case 1:
            return QVariant(QString(std::get<1>(angles_[index.row()]).c_str()));
        case 2:
            return QVariant(QString(std::get<2>(angles_[index.row()]).c_str()));
        case 3:
            return QVariant(std::get<3>(angles_[index.row()]));
        case 4:
            return QVariant(std::get<4>(angles_[index.row()]));
        default:
            return QVariant();
    }
}

QVariant XmlAngleModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return "AtomK";
        case 3:
            return "Angle";
        case 4:
            return "k";
        default:
            return QVariant();
    }
}

std::vector<ForcefieldAngleTerm> XmlAngleModel::toVector()
{
    std::vector<ForcefieldAngleTerm> result;
    for (auto &angle : angles_)
        result.emplace_back(std::get<0>(angle), std::get<1>(angle), std::get<2>(angle), SpeciesAngle::HarmonicForm,
                            std::vector({std::get<4>(angle), std::get<3>(angle)}));
    return result;
}

void XmlAngleModel::clear()
{
    beginResetModel();
    angles_.clear();
    endResetModel();
}
