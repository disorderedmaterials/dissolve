// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/graph.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

class ConfigurationModel : public QAbstractListModel
{
    Q_OBJECT
    // The main graph
    Q_PROPERTY(Graph *graph READ graph WRITE setGraph);

    private:
    // Root graph that we pull nodes from
    Graph *root_;

    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Graph *graph() const;
    void setGraph(Graph *node);
    Q_INVOKABLE void reset();
};
