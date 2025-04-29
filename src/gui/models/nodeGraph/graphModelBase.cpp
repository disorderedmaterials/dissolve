// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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
bool GraphModelBase::disconnect(std::string source, int sourceIndex, std::string destination, int destinationIndex)
{
    GraphRawEdge edge{source, sourceIndex, destination, destinationIndex};

    disconnect_(edge);
    edges_.dropEdge(edge);
    Q_EMIT(nodes()->dataChanged(nodes()->index(indexByName(destination)), nodes()->index(indexByName(destination))));

    return true;
}

// Check whether a given source and destination can be connected
bool GraphModelBase::isValidEdgeSource_(GraphRawEdge &edge) { return false; }

// public wrapper of connect_
bool GraphModelBase::connect(std::string source, int sourceIndex, std::string destination, int destinationIndex)
{
    GraphRawEdge edge{source, sourceIndex, destination, destinationIndex};
    if (!isValidEdgeSource_(edge))
        return false;

    edges()->addEdge(edge);

    connect_(edge);

    Q_EMIT(nodes()->dataChanged(nodes()->index(indexByName(destination)), nodes()->index(indexByName(destination))));
    return true;
}

// The total number of nodes in the graph
int GraphModelBase::count() { return 0; }

// The total number of edges in the graph
int GraphModelBase::nEdges() { return edges_.rowCount(); }
