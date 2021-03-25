#include "gui/models/xmlTreeModel.h"
#include "classes/atomtype.h"
#include "data/elements.h"
#include <QColor>
#include <pugixml.hpp>

XmlTreeIndex::XmlTreeIndex(int row, int column, pugi::xml_node node) : row_(row), column_(column), node_(node) {}

XmlTreeModel::XmlTreeModel(Dissolve &dissolve) : QAbstractItemModel(), dissolve_(dissolve) {}

void XmlTreeModel::readFile(const pugi::xml_node &root)
{
    beginResetModel();
    root_ = root;
    bonds_.readFile(root);
    endResetModel();
}

int XmlTreeModel::rowCount(const QModelIndex &parent) const
{
    switch (parent.internalId())
    {
        case 11:
            return bonds_.rowCount();
        default:
            return 5;
    }
}

int XmlTreeModel::columnCount(const QModelIndex &parent) const { return 16; }

QModelIndex XmlTreeModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    switch (index.internalId())
    {
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
    }
    return createIndex(index.internalId(), 0, index.internalId() - 1);
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
            switch (index.column())
            {
                case 0:
                    return "Name";
                case 1:
                    return "Class";
                case 2:
                    return "Mass";
                case 3:
                    return "Element";
                default:
                    return QVariant();
            }
            break;
        case 11:
            return bonds_.data(bonds_.index(index.row(), index.column()), role);
        case 12:
            switch (index.column())
            {
                case 0:
                    return "Atom I";
                case 1:
                    return "Atom J";
                case 2:
                    return "Atom K";
                case 3:
                    return "Angle";
                case 4:
                    return "k";
                default:
                    return QVariant();
            }
            break;
        case 13:
        case 14:
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
                    return QVariant();
            }
            break;
        default:
            return index.internalId();
    }
}

QModelIndex XmlTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() > 1000)
        return QModelIndex();
    else if (parent.internalId() == 0)
        child = 10 + parent.row();
    else
        child = 100 + parent.internalId();

    return createIndex(row, column, child);
}
