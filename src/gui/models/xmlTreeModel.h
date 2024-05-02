// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/atomType.h"
#include "data/ff/xml/base.h"
#include "gui/models/xmlAngleModel.h"
#include "gui/models/xmlAtomModel.h"
#include "gui/models/xmlBondModel.h"
#include "gui/models/xmlImproperModel.h"
#include "gui/models/xmlTorsionModel.h"
#include "main/dissolve.h"

#include <QAbstractTableModel>
#include <QModelIndex>
#include <pugixml.hpp>

#include <map>
#include <tuple>
#include <vector>

class XmlTreeIndex
{
    private:
    int row_, column_;
    pugi::xml_node node_;

    public:
    XmlTreeIndex(int row, int column, pugi::xml_node node);
    XmlTreeIndex() = default;
};

class XmlTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    private:
    pugi::xml_node root_;
    XmlTreeIndex rootIndex_;
    XmlAtomModel atoms_;
    XmlBondModel bonds_;
    XmlAngleModel angles_;
    XmlTorsionModel torsions_;
    XmlImproperModel impropers_;
    std::string name_;

    public Q_SLOTS:
    // Read from an XML file
    void readFile(const pugi::xml_node &);

    public:
    XmlTreeModel(const CoreData &coreData);

    QModelIndex parent(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    bool hasChildren(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    std::shared_ptr<Forcefield_XML> toForcefield();

    void setName(std::string name);

    bool isValid() const;

    void resetXml();
};
