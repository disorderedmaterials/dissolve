#include "gui/xmlFFModel.h"

XmlFFModel::XmlFFModel() { bonds_ = {{"Foo", "Bar", 1.2, 2.3}, {"Baz", "Quux", 3.4, 4.5}}; }

int XmlFFModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return bonds_.size();
}

int XmlFFModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant XmlFFModel::data(const QModelIndex &index, int role) const
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

QVariant XmlFFModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section)
    {
	case 0:
	    return QVariant(QString("Foo"));
	case 1:
	    return QVariant(QString("Bar"));
	case 2:
	    return QVariant(QString("Quux"));
	default:
	    return QVariant(QString("Xyzzy"));
    }
}
