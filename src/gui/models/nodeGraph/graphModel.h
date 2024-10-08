// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "graphNodeModel.h"
#include "nodeWrapper.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>
#include <variant>
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
    std::vector<std::array<int, 4>> edgeCache;

    Q_SIGNALS:
    void graphChanged();

    public Q_SLOTS:
    virtual void emplace_back(int x, int y, QVariant value) {}
    virtual void deleteNode(int index) {}
    virtual bool connect(int source, int sourceIndex, int destination, int destinationIndex);
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
        graphChanged();
    }
    void deleteNode(int index) override
    {
        nodes_.beginRemoveRows({}, index, index);
        items.erase(items.begin() + index);
        nodes_.endRemoveRows();
        graphChanged();
    }

    bool connect(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        auto &src = items[source].rawValue();
        if (!std::holds_alternative<double>(src.value))
            // Doesn't have a raw value
            return false;

        auto &newEdge = edgeCache.emplace_back();
        edges_.beginResetModel();
        newEdge[0] = source;
        newEdge[1] = sourceIndex;
        newEdge[2] = destination;
        newEdge[3] = destinationIndex;
        edges_.endResetModel();

        items[destination].rawValue().value = &src;
        Q_EMIT(nodes_.dataChanged(nodes_.index(destination), nodes_.index(destination)));
        return true;
    }

    private:
    GraphNodeModel<T> nodes_;
    GraphEdgeModel<T> edges_;
};
