// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "templates/optionalref.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <vector>

class ProcedureModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source ProcedureNode data
    Procedure& procedure_;

    /*
     * QAbstractItemModel overrides
     */
    public:
    ProcedureModel(Procedure &procedure);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
