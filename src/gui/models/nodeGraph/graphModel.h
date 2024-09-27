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
    Q_PROPERTY(int nodeCount READ count NOTIFY graphChanged);

    public:
    GraphModelBase();
    virtual QAbstractListModel *edges();
    virtual QAbstractListModel *nodes();
    virtual int count();

    Q_SIGNALS:
    void graphChanged();

    public Q_SLOTS:
    virtual void emplace_back(int x, int y, QVariant value) {}
    virtual void deleteNode(int index) {}
};

template <typename T> class GraphModel : public GraphModelBase
{
    public:
    GraphModel() : nodes_(this), edges_(this) {}
    std::vector<NodeWrapper<T>> items;

    public:
    QAbstractListModel *edges() override { return &edges_; }
    QAbstractListModel *nodes() override { return &nodes_; }
    int count() override { return nodes_.rowCount(); }
    void emplace_back(int x, int y, QVariant value) override
    {
        nodes_.beginInsertRows({}, items.size(), items.size() + 1);
        auto &item = items.emplace_back(value);
        item.posx = x;
        item.posy = y;
        nodes_.endInsertRows();
        edges_.beginResetModel();
        edges_.endResetModel();
        graphChanged();
    }
    void deleteNode(int index) override
    {
        nodes_.beginRemoveRows({}, index, index);
        items.erase(items.begin() + index);
        nodes_.endRemoveRows();
        edges_.beginResetModel();
        edges_.endResetModel();
        graphChanged();
    }

    private:
    GraphNodeModel<T> nodes_;
    GraphEdgeModel<T> edges_;
};
