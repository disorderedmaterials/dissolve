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
        atoms_.emplace_back(b.node().attribute("name").as_string(), b.node().attribute("class").as_string(),
                            b.node().attribute("element").as_string(), b.node().attribute("mass").as_double());

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
    return 4;
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
    for (auto &at : atoms_)
    {
        auto type = dissolve_.addAtomType(Elements::element(std::get<2>(at)));
        type->setName(std::get<1>(at));
        result.emplace_back(type->Z(), type->index(), type->name(), "", "", type->charge(), type->shortRangeParameters());
    }
    return result;
}
