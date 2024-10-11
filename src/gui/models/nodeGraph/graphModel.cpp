// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "graphModel.h"
#include "graphEdgeModel.h"
#include <qnamespace.h>
#include <qvariant.h>

GraphModelBase::GraphModelBase() {}

GraphEdgeModel *GraphModelBase::edges() { return &edges_; }

QAbstractListModel *GraphModelBase::nodes() { return nullptr; }

bool GraphModelBase::connect_(int source, int sourceIndex, int destination, int destinationIndex) { return false; }

bool GraphModelBase::disconnect_(int source, int sourceIndex, int destination, int destinationIndex) { return false; }

bool GraphModelBase::disconnect(int source, int sourceIndex, int destination, int destinationIndex)
{
    auto &edgeCache = edges_.edgeCache();

    for (int i = edgeCache.size() - 1; i >= 0; --i)
    {
        auto edge = edgeCache[i];

        // Skip wrong edges
        if (source != edge[0] || sourceIndex != edge[1] || destination != edge[2] || destinationIndex != edge[3])
            continue;

        disconnect_(source, sourceIndex, destination, destinationIndex);
        Q_EMIT(nodes()->dataChanged(nodes()->index(destination), nodes()->index(destination)));
        edges_.dropEdge(i);
    }

    return true;

    return false;
}

bool GraphModelBase::isValidEdgeSource_(int source, int sourceIndex, int destination, int destinationIndex) { return false; }

bool GraphModelBase::connect(int source, int sourceIndex, int destination, int destinationIndex)
{
    if (!isValidEdgeSource_(source, sourceIndex, destination, destinationIndex))
        return false;

    edges()->addEdge(source, sourceIndex, destination, destinationIndex);

    connect_(source, sourceIndex, destination, destinationIndex);

    Q_EMIT(nodes()->dataChanged(nodes()->index(destination), nodes()->index(destination)));
    return true;
}

int GraphModelBase::count() { return 0; }

int GraphModelBase::nEdges() { return edges_.rowCount(); }
