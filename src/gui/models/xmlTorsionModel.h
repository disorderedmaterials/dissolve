// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "data/ff/torsionTerm.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <pugixml.hpp>

#include <map>
#include <tuple>
#include <vector>

using XmlTorsionData = std::tuple<std::string, std::string, std::string, std::string, double, double, double, double, int, int,
                                  int, int, double, double, double, double>;

class XmlTorsionModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<XmlTorsionData> torsions_;

    public Q_SLOTS:
    // Read from an XML file
    void readFile(const pugi::xml_node &);

    public:
    XmlTorsionModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void clear();
    std::vector<ForcefieldTorsionTerm> toVector();
};
