// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "graphNodeModel.h"
#include "nodeWrapper.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>
#include <vector>

class GraphModelBase : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(QAbstractListModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(QAbstractListModel *edges READ edges NOTIFY graphChanged);

    public:
    GraphModelBase();
    virtual QAbstractListModel *edges();
    virtual QAbstractListModel *nodes();

    Q_SIGNALS:
    void graphChanged();
};

template <typename T> class GraphModel : public GraphModelBase
{
    public:
    GraphModel() : nodes_(this), edges_(this)
    {
        auto &first = items.emplace_back(7.5);
        first.name = "Source";
        first.posx = 100;
        first.posy = 300;

        auto &second = items.emplace_back(12.5);
        second.name = "Destination";
        second.posx = 600;
        second.posy = 400;
    }
    std::vector<NodeWrapper> items;

    public:
    QAbstractListModel *edges() override { return &edges_; }
    QAbstractListModel *nodes() override { return &nodes_; }

    private:
    GraphNodeModel<T> nodes_;
    GraphEdgeModel<T> edges_;
};
