// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui/models/commonAngleModel.h"
#include "gui/models/commonBondModel.h"
#include "gui/models/commonImproperModel.h"
#include "gui/models/commonTorsionModel.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QObject>

// Master Terms Tree Model
class MasterTermTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
    explicit MasterTermTreeModel(Species *species);

    public:
    // Term models
    CommonBondModel bondModel_;
    CommonAngleModel angleModel_;
    CommonTorsionModel torsionModel_;
    CommonImproperModel improperModel_;

    private:
    CommonTermModel &modelForTopLevelRow(int row);

    public:
    void setBondQueryFunction(std::function<bool(std::string_view termName, Species *sp)> func);
    void setAngleQueryFunction(std::function<bool(std::string_view termName, Species *sp)> func);
    void setTorsionQueryFunction(std::function<bool(std::string_view termName, Species *sp)> func);
    void setImproperQueryFunction(std::function<bool(std::string_view termName, Species *sp)> func);
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
