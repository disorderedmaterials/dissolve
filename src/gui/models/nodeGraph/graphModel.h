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

    public Q_SLOTS:
    virtual void emplace_back(QString name, int x, int y, QVariant value) {}
};

template <typename T> class GraphModel : public GraphModelBase
{
    public:
    GraphModel() : nodes_(this), edges_(this) {}
    std::vector<T> items;

    public:
    QAbstractListModel *edges() override { return &edges_; }
    QAbstractListModel *nodes() override { return &nodes_; }
    void emplace_back(QString name, int x, int y, QVariant value) override
    {
        nodes_.beginInsertRows({}, items.size(), items.size() + 1);
        auto &item = items.emplace_back(value);
        item.name = name.toStdString();
        item.posx = x;
        item.posy = y;
        nodes_.endInsertRows();
        edges_.beginResetModel();
        edges_.endResetModel();
    }

    private:
    GraphNodeModel<T> nodes_;
    GraphEdgeModel<T> edges_;
};
