// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/edge.h"
#include "nodes/graph.h"
#include "nodes/node.h"
#include <QAbstractListModel>
#include <algorithm>
#include <qnamespace.h>

class GraphModel;

/** A model to keep track of the edges between the nodes in the graph.
 * Note that the model only maintains a record of the *existing* edges
 * between nodes in the model.  The GraphModel is the source of truth
 * and this class just caches all of the edges.**/
class GraphEdgeModel : public QAbstractListModel
{
    Q_OBJECT;

    public:
    GraphEdgeModel(GraphModel *parent, Graph *&graph);
    GraphEdgeModel(const GraphEdgeModel &other);

    // Remove an edge from the model (by index). Returns false if edge does not exist
    bool dropEdge(std::size_t edge);

    // Remove an edge by value
    bool dropEdge(Edge &edge);

    void deleteNode(std::string index)
    {
        for (auto &edge : edges())
            if (index == edge->sourceNode().name() || index == edge->targetNode().name())
                dropEdge(*edge);
    }

    // Create a new edge
    void addEdge(Edge &newEdge);

    // Return number of edges (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;

    public Q_SLOTS:
    // Update all edges connected to the node at idx
    void updatePosition(const int idx);

    private:
    // The graph whose edges we model
    Graph *&graph_;
    // The owner of this edge model
    GraphModel *parent_;

    // The edges of the graph
    Graph::Edges &edges();
    const Graph::Edges &edges() const;
};
