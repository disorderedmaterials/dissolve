// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "gui2/models/graphNodeModel.h"
#include "gui2/models/nodeWrapper.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>

class GraphNodeModel;
class GraphEdgeModel;

// This is the base class for any node graph type
class GraphModel : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(Graph *graph READ graph WRITE setGraph NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);
    Q_PROPERTY(QAbstractListModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(int nodeCount READ count NOTIFY graphChanged);
    Q_PROPERTY(int edgeCount READ nEdges NOTIFY graphChanged);
    Q_PROPERTY(QString location READ location NOTIFY graphChanged);
    Q_PROPERTY(bool atRoot READ atRoot NOTIFY graphChanged);

    friend GraphNodeModel;
    friend GraphEdgeModel;

    public:
    GraphModel();

    public:
    // Access the actual nodes in the model
    Graph *graph();

    void setGraph(Graph *graph);

    // The model for the edges in the graph
    GraphEdgeModel *edges();
    // The model for the nodes in the graph
    QAbstractListModel *nodes();
    // The total number of nodes in the graph
    int count();
    // The total number of edges in the graph
    int nEdges();
    // The path to the current graph
    QString location() const;
    // Whether the current graph has a parent
    bool atRoot() const;
    // Add a pre-created node
    void addNode(std::unique_ptr<Node> node, std::string_view name);

    protected:
    // The abstract data model for the nodes
    GraphNodeModel nodes_;
    // The abstract data model for the edges between nodes
    GraphEdgeModel edges_;
    // The graph being modelled
    Graph *graph_;
    // Graph nodes wrapped in the wrappers
    std::vector<NodeWrapper> wrapped_;
    // Get index of name
    int indexByName(std::string_view name);

    private:
    // Check whether a given source and destination can be connected
    bool isValidEdgeSource_(Edge &edge);
    // Connect two nodes
    bool connect_(Edge &edge);
    // Remove a connection
    bool disconnect_(Edge &edge);

    Q_SIGNALS:
    void graphChanged();

    public Q_SLOTS:
    // Reset everything
    void handleReset();
    // Remove a node
    void deleteNode(int index);
    // Select a specific output for connection
    void addEdge(QString srcNode, QString srcOutput, QString tgtNode, QString tgtInput);
    // public wrapper of connect_
    bool connect(std::string source, int sourceIndex, std::string destination, int destinationIndex);
    // Public wrapper of disconnect_
    bool disconnect(std::string source, int sourceIndex, std::string destination, int destinationIndex);

    // Provide relative coordinates for an input on a node
    void addInput(int nodeIndex, QString paramName, double x, double y);
    // Provide relative coordinates for an output on a node
    void addOutput(int nodeIndex, QString paramName, double x, double y);

    // Add a new node at a specific position
    void emplace_back(int x, int y, QVariant type, QVariant name);

    // Switch to parent graph
    void upLevel();
    // Move into an inner graph
    void descend(int index);
};
