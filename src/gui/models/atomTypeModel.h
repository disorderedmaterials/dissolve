// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "templates/optionalref.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>

#include <vector>

class AtomTypeModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source AtomType data
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<AtomType>>> atomTypes_;
    // Icon return function
    std::function<QIcon(const AtomType *atomType)> iconFunction_;

    private:
    // Return object represented by specified model index
    AtomType *rawData(const QModelIndex &index) const;

    public:
    // Set source AtomType data
    void setData(const std::vector<std::shared_ptr<AtomType>> &species);
    // Set function to return QIcon for item
    void setIconFunction(std::function<QIcon(const AtomType *atomType)> func);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
