#include "gui/models/xmlTreeModel.h"
#include "classes/atomtype.h"
#include "data/elements.h"
#include "data/ff/xml/base.h"
#include <QColor>
#include <pugixml.hpp>

XmlTreeIndex::XmlTreeIndex(int row, int column, pugi::xml_node node) : row_(row), column_(column), node_(node) {}

XmlTreeModel::XmlTreeModel(Dissolve &dissolve) : QAbstractItemModel(), atoms_(dissolve) {}

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
            return QModelIndex();
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
            return QModelIndex();
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

    if (role != Qt::DisplayRole)
        return QVariant();
    if (!index.parent().isValid())
    {
        if (index.column() > 0)
            return QVariant();
        switch (index.row())
        {
            case 0:
                return "Atoms";
            case 1:
                return "Bonds";
            case 2:
                return "Angles";
            case 3:
                return "Torsions";
            case 4:
                return "Impropers";
            default:
                return QVariant();
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
        return QModelIndex();

    return createIndex(row, column, child);
}

Forcefield_XML XmlTreeModel::toForcefield()
{
    return Forcefield_XML(atoms_.toVector(), bonds_.toVector(), angles_.toVector(), torsions_.toVector(),
                          impropers_.toVector());
}
