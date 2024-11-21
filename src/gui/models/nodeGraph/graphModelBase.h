// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>

// An abstract class to define the methods needed for a Graph model.
// This will normally include a GraphNodeModel and a GraphEdgeModel.
// However, the GraphNodeModel is a template class, so it cannot be
// included in the base here.  Instead, it is referenced by
// QAbstractListModel for what is needed.
class GraphModelBase : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(QAbstractListModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);
    Q_PROPERTY(int nodeCount READ count NOTIFY graphChanged);
    Q_PROPERTY(int edgeCount READ nEdges NOTIFY graphChanged);

    public:
    GraphModelBase();
    // The model for the edges in the graph
    GraphEdgeModel *edges();
    // The modelfor the nodes in the graph
    virtual QAbstractListModel *nodes();
    // The total number of nodes in the graph
    virtual int count();
    // The total number of edges in the graph
    int nEdges();

    protected:
    GraphEdgeModel edges_;

    private:
    // Check whether a given source and destination can be connected
    virtual bool isValidEdgeSource_(int source, int sourceIndex, int destination, int destinationIndex);
    // Connect two nodes
    virtual bool connect_(int source, int sourceIndex, int destination, int destinationIndex);
    // Remove a connection
    virtual bool disconnect_(int source, int sourceIndex, int destination, int destinationIndex);

    Q_SIGNALS:
    void graphChanged();

    public Q_SLOTS:
    // Add a new node at a specific location
    virtual void emplace_back(int x, int y, QVariant value) {}
    // Remove a node
    virtual void deleteNode(int index) {}
    // public wrapper of connect_
    bool connect(int source, int sourceIndex, int destination, int destinationIndex);
    // Public wrapper of disconnect_
    bool disconnect(int source, int sourceIndex, int destination, int destinationIndex);
};
