// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "templates/optionalref.h"
#include <QAbstractItemModel>
#include <QModelIndex>

#include <vector>

class ProcedureModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
    ProcedureModel(OptionalReferenceWrapper<Procedure> procedure = std::nullopt);

    private:
    // Source ProcedureNode data
    OptionalReferenceWrapper<Procedure> procedure_;

    public:
    // Set source Procedure
    void setData(Procedure &procedure);
    // Reset model data, forcing update
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
