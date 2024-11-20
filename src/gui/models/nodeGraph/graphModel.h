// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphModelBase.h"
#include "nodeWrapper.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>
#include <variant>
#include <vector>

/**
   There's a bit of trickiness because Qt's metaobject compiler won't
   create QObjects from template classes.  The workaround requires
   three steps.

   1) Create a QObject virtual base class that is not templeted.  This
   can define slots and properties we'll need for the later classes.
   Unfortunately, we do not have access to the actual data type, so
   the data has to be wrapped in a QVariant.  This is the
   GraphModelBase class.

   2) Create a template class that inherits from the base.  This class
   can instantiate the virtual methods from the virtual base class,
   since it has access to the actual data.  However, the metaobject
   compiler will not recognize this class as a QObject, since it's a
   template class.  This is the GraphModel<T> class

   3) Create a subclass of a *specialization* of the template class.
   This *can* be a QObject and create its own special properties and
   signals, but still gets access to all of the helper methods from
   the template class.
**/

// This is the base class for any node graph type
template <typename T> class GraphModel : public GraphModelBase
{
    public:
    GraphModel() : nodes_(this) {}

    protected:
    typename GraphNodeContext<T>::type context_;
    // The nodes in the model
    std::vector<NodeWrapper<T>> items_;

    public:
    // Access the acutal nodes in the model
    std::vector<NodeWrapper<T>> &items() { return items_; }
    QAbstractListModel *nodes() override { return &nodes_; }
    int count() override { return nodes_.rowCount(); }
    void emplace_back(int x, int y, QVariant value) override
    {
        nodes_.beginInsertRows({}, items_.size(), items_.size() + 1);
        auto &item = items_.emplace_back(value);
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
        if (nodeDelete((items_.begin() + index)->rawValue(), context_))
            items_.erase(items_.begin() + index);
        nodes_.endRemoveRows();
        graphChanged();
    }

    bool connect_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        return nodeConnect(items_[source].rawValue(), sourceIndex, items_[destination].rawValue(), destinationIndex);
    }

    bool disconnect_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        return nodeDisconnect(items_[source].rawValue(), sourceIndex, items_[destination].rawValue(), destinationIndex);
    }

    bool isValidEdgeSource_(int source, int sourceIndex, int destination, int destinationIndex) override
    {
        return nodeConnectable(items_[source].rawValue(), sourceIndex, items_[destination].rawValue(), destinationIndex);
    }

    protected:
    GraphNodeModel<T> nodes_;
};
