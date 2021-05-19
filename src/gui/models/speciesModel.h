// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "templates/optionalref.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <vector>

class SpeciesModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source Species data
    OptionalReferenceWrapper<const std::vector<std::unique_ptr<Species>>> species_;

    public:
    // Set source Species data
    void setData(const std::vector<std::unique_ptr<Species>> &species);
    // Return object represented by specified model index
    const Species *rawData(const QModelIndex &index) const;

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
