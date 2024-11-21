// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "gui/models/nodeGraph/graphModelBase.h"
#include "gui/models/nodeGraph/graphNodeModel.h"
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

    // Access the GraphNodeModel
    QAbstractListModel *nodes() override { return &nodes_; }

    // The number of nodes
    int count() override { return nodes_.rowCount(); }

    // Add a new node at a specific position
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

    // Remove a node
    void deleteNode(int index) override
    {
        // List of edges to remove
        auto deadEdges = edges_.deleteNode(index);

        for (auto &edge : deadEdges)
        {
            if (edge.source == index)
                Q_EMIT(nodes_.dataChanged(nodes_.index(edge.destination), nodes_.index(edge.destination)));
            disconnect_(edge);
        }

        nodes_.beginRemoveRows({}, index, index);
        if (nodeDelete((items_.begin() + index)->rawValue(), context_))
            items_.erase(items_.begin() + index);
        nodes_.endRemoveRows();
        graphChanged();
    }

    // Create a connection.  Returns true if the connection was made
    bool connect_(GraphRawEdge &edge) override
    {
        return nodeConnect(items_[edge.source].rawValue(), edge.sourceIndex, items_[edge.destination].rawValue(),
                           edge.destinationIndex);
    }

    // Remove a connection.  Returns true if the edge was successfully
    // removed.
    bool disconnect_(GraphRawEdge &edge) override
    {
        return nodeDisconnect(items_[edge.source].rawValue(), edge.sourceIndex, items_[edge.destination].rawValue(),
                              edge.destinationIndex);
    }

    // Determine if two nodes can be connected in the desired way
    bool isValidEdgeSource_(GraphRawEdge &edge) override
    {
        return nodeConnectable(items_[edge.source].rawValue(), edge.sourceIndex, items_[edge.destination].rawValue(),
                               edge.destinationIndex);
    }

    protected:
    // The abstract data model for the nodes
    GraphNodeModel<T> nodes_;
};
