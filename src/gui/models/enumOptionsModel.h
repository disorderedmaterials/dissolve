// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalref.h"
#include <QAbstractListModel>
#include <QModelIndex>
#include <vector>

// Forward Declarations
class EnumOptionsBase;

class EnumOptionsModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source EnumOptions data
    OptionalReferenceWrapper<EnumOptionsBase> enumOptions_;

    public:
    // Set source EnumOptions data
    void setData(EnumOptionsBase &options);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
