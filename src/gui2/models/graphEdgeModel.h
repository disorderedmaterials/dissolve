// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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
    // Edge edit mode
    Q_PROPERTY(bool edgeEditMode READ edgeEditMode NOTIFY edgeEditModeChanged);

    public:
    GraphEdgeModel(GraphModel *parent, Graph *&graph);
    GraphEdgeModel(const GraphEdgeModel &other);

    // Remove an edge from the model (by index). Returns false if edge does not exist
    bool remove(std::size_t edge);
    // Remove an edge by value
    bool remove(Edge &edge);
    // Remove any edges connected to a node with a given name
    void removeConnected(std::string nodeName);
    // Create a new edge
    void add(Edge &newEdge);
    bool add(EdgeDefinition &newEdge);

    public:
    // Toggles the edge edit mode
    Q_INVOKABLE void toggleEdgeEditMode();
    // Returns the current edge edit mode
    Q_INVOKABLE bool edgeEditMode();

    public Q_SLOTS:
    // Update all edges connected to the node at idx
    void updatePosition(const int idx);
    // Drop cache and pull all data from graph
    void reset();

    Q_SIGNALS:
    void edgeEditModeChanged();

    private:
    // The graph whose edges we model
    Graph *&graph_;
    // The owner of this edge model
    GraphModel *parent_;
    // Graph edge edit mode (default to TRUE = add edge, FALSE = remove edge)
    bool edgeEditMode_{true};

    private:
    // The edges of the graph
    Graph::Edges &edges();
    const Graph::Edges &edges() const;

    /*
     * QAbstractListModel overrides
     */

    public:
    // Return number of edges (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;
};
