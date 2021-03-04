#include "models/xmlImproperModel.h"
#include <pugixml.hpp>

XmlImproperModel::XmlImproperModel() {}

void XmlImproperModel::readFile(const QString &file)
{
    pugi::xml_document doc;

    auto result = doc.load_file(file.toStdString().c_str());
    if (!result)
	return;

    auto root = doc.root();

    beginResetModel();
    impropers_.clear();

    for (auto &i : root.select_nodes("/ForceField/PeriodicTorsionForce/Improper"))
    {
	impropers_.emplace_back(i.node().attribute("class1").as_string(), i.node().attribute("class2").as_string(),
			       i.node().attribute("class3").as_string(), i.node().attribute("class4").as_string(),
			       i.node().attribute("k1").as_double(), i.node().attribute("k2").as_double(),
			       i.node().attribute("k3").as_double(), i.node().attribute("k4").as_double(),
			       i.node().attribute("periodicity1").as_int(), i.node().attribute("periodicity2").as_int(),
			       i.node().attribute("periodicity3").as_int(), i.node().attribute("periodicity4").as_int(),
			       i.node().attribute("phase1").as_double(), i.node().attribute("phase2").as_double(),
			       i.node().attribute("phase3").as_double(), i.node().attribute("phase4").as_double());
    }

    endResetModel();
}

int XmlImproperModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return impropers_.size();
}

int XmlImproperModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 16;
}

QVariant XmlImproperModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
	return QVariant();

    switch (index.column())
    {
	case 0:
	    return std::get<0>(impropers_[index.row()]).c_str();
	case 1:
	    return std::get<1>(impropers_[index.row()]).c_str();
	case 2:
	    return std::get<2>(impropers_[index.row()]).c_str();
	case 3:
	    return std::get<3>(impropers_[index.row()]).c_str();

	case 4:
	    return std::get<4>(impropers_[index.row()]);
	case 5:
	    return std::get<5>(impropers_[index.row()]);
	case 6:
	    return std::get<6>(impropers_[index.row()]);
	case 7:
	    return std::get<7>(impropers_[index.row()]);

	case 8:
	    return std::get<8>(impropers_[index.row()]);
	case 9:
	    return std::get<9>(impropers_[index.row()]);
	case 10:
	    return std::get<10>(impropers_[index.row()]);
	case 11:
	    return std::get<11>(impropers_[index.row()]);

	case 12:
	    return std::get<12>(impropers_[index.row()]);
	case 13:
	    return std::get<13>(impropers_[index.row()]);
	case 14:
	    return std::get<14>(impropers_[index.row()]);
	case 15:
	    return std::get<15>(impropers_[index.row()]);

	default:
	    return QVariant();
    }
}

QVariant XmlImproperModel::headerData(int section, Qt::Orientation orientation, int role) const
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
}
