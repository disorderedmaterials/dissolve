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
    Procedure &procedure_;
    std::vector<std::pair<std::string, KeywordBase *>> getKeywords_() const;

    /*
     * QAbstractItemModel overrides
     */
    public:
    ProcedureModel(Procedure &procedure);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
