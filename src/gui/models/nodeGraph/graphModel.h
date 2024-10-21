// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphModelBase.h"
#include "nodeWrapper.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>
#include <variant>
#include <vector>

template <typename T> class GraphModel : public GraphModelBase
{
    public:
    GraphModel() : nodes_(this) {}
    std::vector<NodeWrapper<T>> items;

    public:
    QAbstractListModel *nodes() override { return &nodes_; }
    int count() override { return nodes_.rowCount(); }
    void emplace_back(int x, int y, QVariant value) override
    {
        nodes_.beginInsertRows({}, items.size(), items.size() + 1);
        auto &item = items.emplace_back(value);
        item.posx = x;
        item.posy = y;
        setNodeName(item.rawValue(), "Unnamed");
        nodes_.endInsertRows();
        graphChanged();
    }
    void deleteNode(int index) override
    {
        // List of edges to remove
        std::vector<int> deadEdges;
        auto &edgeCache = edges_.edgeCache();

        // Find connected edges and update nodes
        for (auto i = 0; i < edgeCache.size(); ++i)
        {
            if (edgeCache[i][0] == index)
            {
                deadEdges.emplace_back(i);
                auto destination = edgeCache[i][2];
                disconnect_(edgeCache[i][0], edgeCache[i][1], edgeCache[i][2], edgeCache[i][3]);
                Q_EMIT(nodes_.dataChanged(nodes_.index(destination), nodes_.index(destination)));
            }
            else if (edgeCache[i][2] == index)
            {
                deadEdges.emplace_back(i);
            }

            // Update indices
            if (edgeCache[i][0] > index)
                --edgeCache[i][0];
            if (edgeCache[i][2] > index)
                --edgeCache[i][2];
        }

        // Remove dead edges
        if (!deadEdges.empty())
        {
            for (int i = deadEdges.size() - 1; i >= 0; --i)
            {
                auto edge = deadEdges[i];
                edges_.dropEdge(edge);
            }
        }

        nodes_.beginRemoveRows({}, index, index);
        items.erase(items.begin() + index);
        nodes_.endRemoveRows();
        graphChanged();
    }

    bool connect_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        return nodeConnect(items[source].rawValue(), sourceIndex, items[destination].rawValue(), destinationIndex);
    }

    bool disconnect_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        return nodeDisconnect(items[source].rawValue(), sourceIndex, items[destination].rawValue(), destinationIndex);
    }

    bool isValidEdgeSource_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        return nodeConnectable(items[source].rawValue(), sourceIndex, items[destination].rawValue(), destinationIndex);
    }

    protected:
    GraphNodeModel<T> nodes_;
};
