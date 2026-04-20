// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/coreData.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <QObject>

namespace CommonTermModelData
{
// Enumerated data items for terms
enum DataType
{
    Name = 0,
    Form = 1,
    Parameters = 2,
    Electrostatic14Scale = 3,
    VanDerWaals14Scale = 4
};

enum Roles
{
    Raw = Qt::UserRole,
    Query = Qt::UserRole + 1
};
}; // namespace CommonTermModelData

// Base common term model
class CommonTermModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    CommonTermModel(Species *species);

    protected:
    // Source Species
    Species *species_{nullptr};
    // Icon return function
    std::function<bool(std::string_view termName, Species* sp)> queryFunction_;

    public:
    // Return model data, by type, for specified term index
    virtual QVariant getTermData(int row, CommonTermModelData::DataType dataType) const = 0;
    // Set model data, by type, for specified term index
    virtual bool setTermData(int row, CommonTermModelData::DataType dataType, const QVariant &value) = 0;
    // Set function to return QIcon for item
    void setQueryFunction(std::function<bool(std::string_view termName)> func);

    /*
     * QAbstractItemModel overrides
     */
    public:
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;
};
