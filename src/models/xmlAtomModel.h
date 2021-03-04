// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"

#include <QAbstractTableModel>
#include <QModelIndex>
#include <pugixml.hpp>

#include <tuple>
#include <vector>

using XmlAtomData = std::tuple<std::string, std::string, std::string, double, int>;

class XmlAtomModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<XmlAtomData> atoms_;
    Dissolve &dissolve_;

    public slots:
    // Read from an XML file
    void readFile(const pugi::xml_node &);

    public:
    XmlAtomModel(Dissolve &);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void clear();
};
