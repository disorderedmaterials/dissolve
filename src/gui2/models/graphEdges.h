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

/** An object to keep track of the edges between the nodes in the graph.
 * Note that the object only maintains a record of the *existing* edges
 * between nodes.  The GraphModel is the source of truth
 * and this class just caches all of the edges.**/
class GraphEdges : public QObject
{
    Q_OBJECT;
    // Edge edit mode
    Q_PROPERTY(bool edgeEditMode READ edgeEditMode NOTIFY edgeEditModeChanged);

    public:
    GraphEdges(GraphModel *parent, Graph *&graph);
    GraphEdges(const GraphEdges &other);

    private:
    // Loop edges belonging to the graph
    Graph::Edges *loop_;
    // Standard edges belonging to the graph
    Graph::Edges *standard_;

    public:
    // Return the parent graph
    Graph *&graph();
    // Return loop edges belonging to the graph
    const Graph::Edges *loop() const;
    // Return standard edges belonging to the graph
    const Graph::Edges *standard() const;
    // Set the loop edges belonging to the graph
    void setLoopEdges(Graph::Edges *edges);
    // Set the standard edges belonging to the graph
    void setEdges(Graph::Edges *edges);
    //
    // std::vector<const Edge *>::iterator find(const Edge *edge);
    // Remove an edge. Returns false if the edge does not exist
    bool remove(Edge *edge);
    // Remove any edges connected to a node with a given name
    void removeConnected(std::string nodeName);
    // Create a new edge. Returns false if the edge could not be created
    bool add(EdgeDefinition &definition);

    public:
    // Toggles the edge edit mode
    Q_INVOKABLE void toggleEdgeEditMode();
    // Returns the current edge edit mode
    Q_INVOKABLE bool edgeEditMode();

    public Q_SLOTS:
    // Return the total edge count (combined number of standard and loop edges)
    int count();

    Q_SIGNALS:
    void edgeEditModeChanged();

    private:
    // The graph whose edges we model
    Graph *&graph_;
    // The owner of this edge model
    GraphModel *parent_;
    // Graph edge edit mode (default to TRUE = add edge, FALSE = remove edge)
    bool edgeEditMode_{true};
};
