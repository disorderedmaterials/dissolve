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
                items[destination].rawValue().value = nullptr;
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

        items[destination].rawValue().value = &items[source].rawValue();
        return true;
    }

    bool disconnect_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        items[destination].rawValue().value = nullptr;
        return true;
    }

    bool isValidEdgeSource_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        auto &src = items[source].rawValue();
        return std::holds_alternative<double>(src.value);
    }

    private:
    GraphNodeModel<T> nodes_;
};
