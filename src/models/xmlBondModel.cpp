#include "models/xmlBondModel.h"
#include <pugixml.hpp>

XmlBondModel::XmlBondModel() {}

void XmlBondModel::readFile(const QString &file)
{
    pugi::xml_document doc;

    auto result = doc.load_file(file.toStdString().c_str());
    if (!result)
	return;

    auto root = doc.root();

    beginResetModel();
    bonds_.clear();

    for (auto &b : root.select_nodes("/ForceField/HarmonicBondForce/Bond"))
    {
	bonds_.emplace_back(b.node().attribute("class1").as_string(), b.node().attribute("class2").as_string(),
			    b.node().attribute("k").as_double(), b.node().attribute("length").as_double());
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
    switch (section)
    {
	case 0:
	    return QVariant(QString("AtomI"));
	case 1:
	    return QVariant(QString("AtomJ"));
	case 2:
	    return QVariant(QString("k"));
	default:
	    return QVariant(QString("Length"));
    }
}
