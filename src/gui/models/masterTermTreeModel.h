// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/masterAngleModel.h"
#include "gui/models/masterBondModel.h"
#include "gui/models/masterImproperModel.h"
#include "gui/models/masterTorsionModel.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <QObject>
#include <vector>

// Master Terms Tree Model
class MasterTermTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
    // Term models
    MasterBondModel bondModel_;
    MasterAngleModel angleModel_;
    MasterTorsionModel torsionModel_;
    MasterImproperModel improperModel_;

    private:
    MasterTermModel &modelForTopLevelRow(int row);

    public:
    void setData(std::vector<std::shared_ptr<MasterBond>> &bonds, std::vector<std::shared_ptr<MasterAngle>> &angles,
                 std::vector<std::shared_ptr<MasterTorsion>> &torsions,
                 std::vector<std::shared_ptr<MasterImproper>> &impropers);
    void setBondIconFunction(std::function<bool(std::string_view termName)> func);
    void setAngleIconFunction(std::function<bool(std::string_view termName)> func);
    void setTorsionIconFunction(std::function<bool(std::string_view termName)> func);
    void setImproperIconFunction(std::function<bool(std::string_view termName)> func);
    void prefixNames(QList<QModelIndex> indices, QString prefix);
    void suffixNames(QList<QModelIndex> indices, QString suffix);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
