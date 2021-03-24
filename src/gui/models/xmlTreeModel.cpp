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
    endResetModel();
}

int XmlTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.internalId() == 0)
        return 4;
    return 2;
}

int XmlTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.internalId() == 0)
        return 4;
    return 1;
}

QModelIndex XmlTreeModel::parent(const QModelIndex &index) const
{
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId(), 0, index.internalId() - 1);
}

QVariant XmlTreeModel::data(const QModelIndex &index, int role) const
{

    if (role != Qt::DisplayRole)
        return QVariant();
    switch (index.column())
    {
        case 0:
            return "Hello";
        case 1:
            return "World";
        default:
            return "Ooops";
    }
}

QModelIndex XmlTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else
      if (parent.internalId() == 3)
	return QModelIndex();
    child = parent.internalId() + 1;

    return createIndex(row, column, child);
}
