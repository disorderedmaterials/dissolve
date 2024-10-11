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

bool GraphModelBase::disconnect(int source, int sourceIndex, int destination, int destinationIndex) { return false; }

bool GraphModelBase::isValidEdgeSource_(int source, int sourceIndex) { return false; }

bool GraphModelBase::connect(int source, int sourceIndex, int destination, int destinationIndex)
{
    if (!isValidEdgeSource_(source, sourceIndex))
        return false;

    edges()->addEdge(source, sourceIndex, destination, destinationIndex);

    connect_(source, sourceIndex, destination, destinationIndex);

    Q_EMIT(nodes()->dataChanged(nodes()->index(destination), nodes()->index(destination)));
    return true;
}

int GraphModelBase::count() { return 0; }

int GraphModelBase::nEdges() { return edges_.rowCount(); }
