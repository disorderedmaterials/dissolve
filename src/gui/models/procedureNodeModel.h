// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <vector>

class ProcedureNodeModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source ProcedureNode data
    std::vector<ConstNodeRef> nodes_;
    // Node selected/deselected/presence functions
    std::function<bool(ConstNodeRef)> nodeSelectedFunction_, nodeDeselectedFunction_, nodePresenceFunction_;

    public:
    // Set source ProcedureNode data
    void setData(const std::vector<ConstNodeRef> &nodes);
    // Set node selected function
    void setNodeSelectedFunction(std::function<bool(ConstNodeRef)> nodeSelectedFunction);
    // Set node selected function
    void setNodeDeselectedFunction(std::function<bool(ConstNodeRef)> nodeDeselectedFunction);
    // Set node presence function
    void setNodePresenceFunction(std::function<bool(ConstNodeRef)> nodePresenceFunction);
    // Return object represented by specified model index
    ConstNodeRef rawData(const QModelIndex &index) const;

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
