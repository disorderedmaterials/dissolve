// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <QDialog>

#include <QAbstractTableModel>
#include <QModelIndex>

#include <tuple>
#include <vector>

class XmlFFModel : public QAbstractTableModel
{
    private:
    std::vector<std::tuple<std::string, std::string, double, double>> bonds_;

    public:
    XmlFFModel() { bonds_ = {{"Foo", "Bar", 1.2, 2.3}, {"Baz", "Quux", 3.4, 4.5}}; }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
	Q_UNUSED(parent);
	return bonds_.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
	Q_UNUSED(parent);
	return 4;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
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
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
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
};
