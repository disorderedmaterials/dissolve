// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "graphEdgeModel.h"
#include "graphModel.h"
#include <qnamespace.h>
#include <qvariant.h>

GraphModelBase::GraphModelBase() {}

// The model for the edges in the graph
GraphEdgeModel *GraphModelBase::edges() { return &edges_; }

// The modelfor the nodes in the graph
QAbstractListModel *GraphModelBase::nodes() { return nullptr; }

// Connect two nodes
bool GraphModelBase::connect_(GraphRawEdge &edge) { return false; }

// Remove a connection
bool GraphModelBase::disconnect_(GraphRawEdge &edge) { return false; }

// Public wrapper of disconnect_
bool GraphModelBase::disconnect(int source, int sourceIndex, int destination, int destinationIndex)
{
    auto &edgeCache = edges_.edgeCache();
    GraphRawEdge edge{source, sourceIndex, destination, destinationIndex};

    for (int i = edgeCache.size() - 1; i >= 0; --i)
    {
        auto e = edgeCache[i];

        if (edge == e)
        {

            disconnect_(edge);
            Q_EMIT(nodes()->dataChanged(nodes()->index(destination), nodes()->index(destination)));
            edges_.dropEdge(i);
        }
    }

    return true;
}

// Check whether a given source and destination can be connected
bool GraphModelBase::isValidEdgeSource_(GraphRawEdge &edge) { return false; }

// public wrapper of connect_
bool GraphModelBase::connect(int source, int sourceIndex, int destination, int destinationIndex)
{
    GraphRawEdge edge{source, sourceIndex, destination, destinationIndex};
    if (!isValidEdgeSource_(edge))
        return false;

    edges()->addEdge(edge);

    connect_(edge);

    Q_EMIT(nodes()->dataChanged(nodes()->index(destination), nodes()->index(destination)));
    return true;
}

// The total number of nodes in the graph
int GraphModelBase::count() { return 0; }

// The total number of edges in the graph
int GraphModelBase::nEdges() { return edges_.rowCount(); }
