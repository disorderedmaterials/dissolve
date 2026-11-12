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
 * Note that the model only maintains a record of the *existing* edges
 * between nodes in the model.  The GraphModel is the source of truth
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
    //
    std::vector<Edge *> loop_;
    //
    std::vector<Edge *> standard_;

    public:
    //
    Graph *&graph();
    //
    const std::vector<Edge *> &loop() const;
    //
    const std::vector<Edge *> &standard() const;
    //
    std::vector<const Edge *>::iterator find(const Edge *edge);
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
    //
    void update();
    //
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
