#include "gui/xmlAtomModel.h"
#include <pugixml.hpp>

XmlAtomModel::XmlAtomModel() {}

void XmlAtomModel::readFile(const QString &file)
{
    pugi::xml_document doc;

    auto result = doc.load_file(file.toStdString().c_str());
    if (!result)
	return;

    auto root = doc.root();

    beginResetModel();
    atoms_.clear();

    for (auto &b : root.select_nodes("/ForceField/AtomTypes/Type"))
    {
	atoms_.emplace_back(b.node().attribute("name").as_string(), b.node().attribute("class").as_string(),
			    b.node().attribute("element").as_string(), b.node().attribute("mass").as_double());
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
    return 4;
}

QVariant XmlAtomModel::data(const QModelIndex &index, int role) const
{
    switch (index.column())
    {
	case 0:
	    return QVariant(QString(std::get<0>(atoms_[index.row()]).c_str()));
	case 1:
	    return QVariant(QString(std::get<1>(atoms_[index.row()]).c_str()));
	case 2:
	    return QVariant(QString(std::get<2>(atoms_[index.row()]).c_str()));
	case 3:
	    return QVariant(std::get<3>(atoms_[index.row()]));
	default:
	    return QVariant();
    }
}

QVariant XmlAtomModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section)
    {
	case 0:
	    return QVariant(QString("Name"));
	case 1:
	    return QVariant(QString("Class"));
	case 2:
	    return QVariant(QString("Element"));
	default:
	    return QVariant(QString("Mass"));
    }
}
