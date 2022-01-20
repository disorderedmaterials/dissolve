// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "templates/optionalref.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <vector>

class ProcedureNodeModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source ProcedureNode data
    std::vector<const ProcedureNode *> nodes_;
    // Vector containing checked items (if relevant)
    OptionalReferenceWrapper<std::vector<const ProcedureNode *>> checkedItems_;

    public:
    // Set source ProcedureNode data
    void setData(const std::vector<const ProcedureNode *> &nodes);
    // Set vector containing checked items
    void setCheckStateData(std::vector<const ProcedureNode *> &checkedItemsVector);
    // Return object represented by specified model index
    const ProcedureNode *rawData(const QModelIndex &index) const;

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
