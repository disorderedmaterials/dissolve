#include "models/xmlAngleModel.h"
#include <pugixml.hpp>

XmlAngleModel::XmlAngleModel() {}

void XmlAngleModel::readFile(const QString &file)
{
    pugi::xml_document doc;

    auto result = doc.load_file(file.toStdString().c_str());
    if (!result)
	return;

    auto root = doc.root();

    beginResetModel();
    angles_.clear();

    for (auto &a : root.select_nodes("/ForceField/HarmonicAngleForce/Angle"))
    {
	angles_.emplace_back(a.node().attribute("class1").as_string(), a.node().attribute("class2").as_string(),
			     a.node().attribute("class3").as_string(), a.node().attribute("k").as_double(),
			     a.node().attribute("angle").as_double());
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
    switch (section)
    {
	case 0:
	    return QVariant(QString("AtomI"));
	case 1:
	    return QVariant(QString("AtomJ"));
	case 3:
	    return QVariant(QString("AtomK"));
	case 4:
	    return QVariant(QString("k"));
	default:
	    return QVariant(QString("angle"));
    }
}
