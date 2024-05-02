#include "gui/models/xmlTreeModel.h"
#include "classes/atomType.h"
#include "data/elements.h"
#include "data/ff/xml/base.h"
#include <QColor>
#include <pugixml.hpp>

XmlTreeIndex::XmlTreeIndex(int row, int column, pugi::xml_node node) : row_(row), column_(column), node_(node) {}

XmlTreeModel::XmlTreeModel(const CoreData &coreData) : QAbstractItemModel(), atoms_(coreData) {}

void XmlTreeModel::readFile(const pugi::xml_node &root)
{
    beginResetModel();
    root_ = root;
    atoms_.readFile(root);
    bonds_.readFile(root);
    angles_.readFile(root);
    torsions_.readFile(root);
    impropers_.readFile(root);
    endResetModel();
}

int XmlTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 5;
    switch (parent.row())
    {
        case 0:
            return atoms_.rowCount();
        case 1:
            return bonds_.rowCount();
        case 2:
            return angles_.rowCount();
        case 3:
            return torsions_.rowCount();
        case 4:
            return impropers_.rowCount();
        default:
            return 0;
    }
}

int XmlTreeModel::columnCount(const QModelIndex &parent) const { return 16; }

QModelIndex XmlTreeModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    switch (index.internalId())
    {
        case 0:
            return {};
        case 10:
            return createIndex(0, 0, root);
        case 11:
            return createIndex(1, 0, root);
        case 12:
            return createIndex(2, 0, root);
        case 13:
            return createIndex(3, 0, root);
        case 14:
            return createIndex(4, 0, root);
        default:
            return {};
    }
}

bool XmlTreeModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;
    return !parent.parent().isValid();
}

QVariant XmlTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.parent().isValid())
    {
        if (role != Qt::DisplayRole)
            return {};
        if (index.column() > 0)
            return {};
        switch (index.row())
        {
            case 0:
                return QString::fromStdString(fmt::format("Atom Types ({})", atoms_.rowCount()));
            case 1:
                return QString::fromStdString(fmt::format("Bonds ({})", bonds_.rowCount()));
            case 2:
                return QString::fromStdString(fmt::format("Angles ({})", angles_.rowCount()));
            case 3:
                return QString::fromStdString(fmt::format("Torsions ({})", torsions_.rowCount()));
            case 4:
                return QString::fromStdString(fmt::format("Impropers ({})", impropers_.rowCount()));
            default:
                return {};
        }
    }
    switch (index.internalId())
    {
        case 10:
            return atoms_.data(atoms_.index(index.row(), index.column()), role);
        case 11:
            return bonds_.data(bonds_.index(index.row(), index.column()), role);
        case 12:
            return angles_.data(angles_.index(index.row(), index.column()), role);
        case 13:
            return torsions_.data(torsions_.index(index.row(), index.column()), role);
        case 14:
            return impropers_.data(impropers_.index(index.row(), index.column()), role);
        default:
            return index.internalId();
    }
}

QModelIndex XmlTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = 10 + parent.row();
    else
        return {};

    return createIndex(row, column, child);
}

std::shared_ptr<Forcefield_XML> XmlTreeModel::toForcefield()
{
    auto ff = std::make_shared<Forcefield_XML>(name_, atoms_.toVector(), bonds_.toVector(), angles_.toVector(),
                                               torsions_.toVector(), impropers_.toVector());
    ff->prepare();
    return ff;
}

QVariant XmlTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};
    return "";
}

void XmlTreeModel::setName(std::string name) { name_ = name; }

bool XmlTreeModel::isValid() const { return name_.length() > 0 && atoms_.rowCount() > 0; }

void XmlTreeModel::resetXml()
{
    beginResetModel();
    atoms_.clear();
    bonds_.clear();
    angles_.clear();
    torsions_.clear();
    impropers_.clear();
    endResetModel();
}
