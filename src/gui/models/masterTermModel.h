// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "templates/optionalref.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <QObject>

#include <vector>

// Base master term model
class MasterTermModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    MasterTermModel(std::vector<std::shared_ptr<MasterIntra>> intras, QObject *parent = nullptr);
    /*
     * QAbstractItemModel overrides
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void reset(std::vector<std::shared_ptr<MasterIntra>> &intras);

    // Set function to return QIcon for item
    void setIconFunction(std::function<QIcon(const MasterIntra *term)> func);

    protected:
    // Source MasterIntra data
    std::vector<std::shared_ptr<MasterIntra>> intras_;

    private:
    // Icon return function
    std::function<QIcon(const MasterIntra *term)> iconFunction_;
};

class MasterTermBondModel : public MasterTermModel
{
    public:
    MasterTermBondModel(std::vector<std::shared_ptr<MasterIntra>> intras, QObject *parent = nullptr)
        : MasterTermModel(std::move(intras), parent)
    {
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
class MasterTermAngleModel : public MasterTermModel
{
    public:
    MasterTermAngleModel(std::vector<std::shared_ptr<MasterIntra>> intras, QObject *parent = nullptr)
        : MasterTermModel(std::move(intras), parent)
    {
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};

class MasterTermTorsionModel : public MasterTermModel
{
    public:
    MasterTermTorsionModel(std::vector<std::shared_ptr<MasterIntra>> intras, QObject *parent = nullptr)
        : MasterTermModel(std::move(intras), parent)
    {
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
