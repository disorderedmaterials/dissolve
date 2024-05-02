// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/xmlTorsionModel.h"
#include <algorithm>
#include <pugixml.hpp>

XmlTorsionModel::XmlTorsionModel() {}

void XmlTorsionModel::readFile(const pugi::xml_node &root)
{
    beginResetModel();
    torsions_.clear();

    for (auto &t : root.select_nodes("/ForceField/PeriodicTorsionForce/Proper"))
    {
        torsions_.emplace_back(t.node().attribute("class1").as_string(), t.node().attribute("class2").as_string(),
                               t.node().attribute("class3").as_string(), t.node().attribute("class4").as_string(),
                               t.node().attribute("k1").as_double(), t.node().attribute("k2").as_double(),
                               t.node().attribute("k3").as_double(), t.node().attribute("k4").as_double(),
                               t.node().attribute("periodicity1").as_int(), t.node().attribute("periodicity2").as_int(),
                               t.node().attribute("periodicity3").as_int(), t.node().attribute("periodicity4").as_int(),
                               t.node().attribute("phase1").as_double(), t.node().attribute("phase2").as_double(),
                               t.node().attribute("phase3").as_double(), t.node().attribute("phase4").as_double());
    }

    endResetModel();
}

int XmlTorsionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return torsions_.size();
}

int XmlTorsionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 16;
}

QVariant XmlTorsionModel::data(const QModelIndex &index, int role) const
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
                return "AtomL";

            case 4:
                return "k 1";
            case 5:
                return "k 2";
            case 6:
                return "k 3";
            case 7:
                return "k 4";

            case 8:
                return "Periodicity 1";
            case 9:
                return "Periodicity 2";
            case 10:
                return "Periodicity 3";
            case 11:
                return "Periodicity 4";

            case 12:
                return "phase1";
            case 13:
                return "phase2";
            case 14:
                return "phase3";
            case 15:
                return "phase4";

            default:
                return {};
        }

    if (role != Qt::DisplayRole)
        return {};

    switch (index.column())
    {
        case 0:
            return QString::fromStdString(std::get<0>(torsions_[index.row()]));
        case 1:
            return QString::fromStdString(std::get<1>(torsions_[index.row()]));
        case 2:
            return QString::fromStdString(std::get<2>(torsions_[index.row()]));
        case 3:
            return QString::fromStdString(std::get<3>(torsions_[index.row()]));

        case 4:
            return std::get<4>(torsions_[index.row()]);
        case 5:
            return std::get<5>(torsions_[index.row()]);
        case 6:
            return std::get<6>(torsions_[index.row()]);
        case 7:
            return std::get<7>(torsions_[index.row()]);

        case 8:
            return std::get<8>(torsions_[index.row()]);
        case 9:
            return std::get<9>(torsions_[index.row()]);
        case 10:
            return std::get<10>(torsions_[index.row()]);
        case 11:
            return std::get<11>(torsions_[index.row()]);

        case 12:
            return std::get<12>(torsions_[index.row()]);
        case 13:
            return std::get<13>(torsions_[index.row()]);
        case 14:
            return std::get<14>(torsions_[index.row()]);
        case 15:
            return std::get<15>(torsions_[index.row()]);

        default:
            return {};
    }
}

QVariant XmlTorsionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    switch (section)
    {
        case 0:
            return "AtomI";
        case 1:
            return "AtomJ";
        case 2:
            return "AtomK";
        case 3:
            return "AtomL";

        case 4:
            return "k 1";
        case 5:
            return "k 2";
        case 6:
            return "k 3";
        case 7:
            return "k 4";

        case 8:
            return "Periodicity 1";
        case 9:
            return "Periodicity 2";
        case 10:
            return "Periodicity 3";
        case 11:
            return "Periodicity 4";

        case 12:
            return "phase1";
        case 13:
            return "phase2";
        case 14:
            return "phase3";
        case 15:
            return "phase4";

        default:
            return {};
    }
}

std::vector<ForcefieldTorsionTerm> XmlTorsionModel::toVector()
{
    std::vector<ForcefieldTorsionTerm> result;
    for (auto &torsion : torsions_)
    {
        std::vector<double> ks({std::get<4>(torsion), std::get<5>(torsion), std::get<6>(torsion), std::get<7>(torsion)}),
            phases({std::get<12>(torsion), std::get<13>(torsion), std::get<14>(torsion), std::get<15>(torsion)});
        std::vector<int> ns({std::get<8>(torsion), std::get<9>(torsion), std::get<10>(torsion), std::get<11>(torsion)});

        std::string paramString;
        auto index = 0;
        for (auto n : ns)
        {
            paramString += fmt::format("k{}={} ", n, (phases[index] > 3.14 && phases[index] < 3.15 ? -1 : 1) * ks[index]);

            ++index;
        }

        result.emplace_back(std::get<0>(torsion), std::get<1>(torsion), std::get<2>(torsion), std::get<3>(torsion),
                            TorsionFunctions::Form::CosN, paramString);
    }
    return result;
}

void XmlTorsionModel::clear()
{
    beginResetModel();
    torsions_.clear();
    endResetModel();
}
