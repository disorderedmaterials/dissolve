// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "gui2/models/graphNodeModel.h"
#include "gui2/models/nodeWrapper.h"
#include "gui2/models/parameterEndPointsModel.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include <QAbstractListModel>
#include <QQuickItem>
#include <functional>
#include <qtmetamacros.h>
#include <random>

class ParameterEndPointsModel;
class GraphNodeModel;
class GraphEdgeModel;

// This is the base class for any node graph type
class GraphModel : public QObject
{
    Q_OBJECT;
    // Read-only graph properties
    Q_PROPERTY(int nodeCount READ count NOTIFY graphChanged);
    Q_PROPERTY(int edgeCount READ nEdges NOTIFY graphChanged);
    Q_PROPERTY(QString location READ location NOTIFY graphChanged);
    Q_PROPERTY(bool atRoot READ atRoot NOTIFY graphChanged);

    // Read-only graph models
    Q_PROPERTY(ParameterEndPointsModel *parameterEndPoints READ parameterEndPoints NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);
    Q_PROPERTY(GraphNodeModel *nodes READ nodes NOTIFY graphChanged);

    // Graph object
    Q_PROPERTY(Graph *graph READ graph WRITE setGraph NOTIFY graphChanged);

    // TODO: Graph view a zoomable/scrollable view? If only scrolling allowed the signal may not be needed
    Q_PROPERTY(QSizeF canvasDimensions READ canvasDimensions WRITE setCanvasDimensions NOTIFY canvasDimensionsChanged);

    friend GraphNodeModel;
    friend GraphEdgeModel;

    public:
    using NodeParameterEndPointsMap = std::map<const Node *, std::map<std::string, QQuickItem *>>;
    GraphModel();

    public:
    // Access the actual nodes in the model
    Graph *graph();
    // Set the current graph being modelled
    void setGraph(Graph *graph);
    // The model for the edge curve end points corresponding to parameter drop areas
    ParameterEndPointsModel *parameterEndPoints();
    // The model for the edges in the graph
    GraphEdgeModel *edges();
    // The model for the nodes in the graph
    GraphNodeModel *nodes();
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
    // Return graph canvas dimensions
    QSizeF canvasDimensions() const;
    // Set graph canvas dimensions
    void setCanvasDimensions(const QSizeF &canvasDimensions);

    private:
    // List of node names corresponding to the current graph's existing nodes, that will be reconstructed
    std::optional<std::vector<std::string>> reconstructibleNodes_;
    // Random number generator
    inline static std::mt19937 rnG_{std::random_device{}()};
    // Graph canvas dimensions
    QSizeF canvasDimensions_;
    // Rendered edge curve endpoints for all inputs
    NodeParameterEndPointsMap curveInputEndPoints_;
    // Rendered edge curve endpoints for all outputs
    NodeParameterEndPointsMap curveOutputEndPoints_;

    protected:
    //
    std::set<Graph *> knownGraphSet_;
    // The abstract data model for the parameter endpoints
    ParameterEndPointsModel parameterEndPoints_;
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

    public:
    // Returns bool - true if we are currently reconstructing existing nodes in the current graph
    Q_INVOKABLE bool nodeReconstructionInProgress();
    // Record that a node with a given name has been reconstructed
    Q_INVOKABLE void reconstructed(QString constructedName);
    // Returns bool - true if the node's parameter is connected to anything
    Q_INVOKABLE bool hasConnections(QString nodeName, QString paramName);
    // Add endpoints between the source node and its output to a target node and its input, where the edge connection has
    // previously been deferred
    Q_INVOKABLE void addDeferredEndPoints();
    // Map an available input endpoint for a node parameter to an input QML DropArea
    Q_INVOKABLE void mapInputEndPoint(QVariant nodeName, QVariant paramName, QQuickItem *endPoint);
    // Map an available output endpoint for a node parameter to an output QML DropArea
    Q_INVOKABLE void mapOutputEndPoint(QVariant nodeName, QVariant paramName, QQuickItem *endPoint);
    // Return bool - true if node exists in graph
    Q_INVOKABLE bool isValidNode(QVariant nodeName) const;
    // Return bool - true if graph run was successful/unchanged, false if failed
    Q_INVOKABLE void run(QVariant nodeName);

    private:
    // Add endpoints between the source node and its output to a target node and its input
    void addEndPoints(std::string sourceNodeName, std::string sourceParamName, std::string targetNodeName,
                      std::string targetParamName);
    // Find a unique point in the graph's x-y space for positioning when instantiated
    void findUniqueXY(int x, int y, int &dX, int &dY);

    /* UNUSED
    private:
    // Check whether a given source and destination can be connected
    bool isValidEdgeSource_(Edge &edge);
    // Connect two nodes
    bool connect_(Edge &edge);
    // Remove a connection
    bool disconnect_(Edge &edge);
    */

    Q_SIGNALS:
    void graphChanged();
    void canvasDimensionsChanged();
    void decrementNodeTypeRequired(const std::string &);
    void graphRunComplete(NodeConstants::ProcessResult status, std::string node);

    public Q_SLOTS:
    // Reset everything
    void handleReset();
    // Remove a node
    void deleteNode(int index);
    // Select a specific output for connection
    void addEdge(QString srcNode, QString srcOutput, QString tgtNode, QString tgtInput);
    // Adds a new edge, but the connection (addition of QML endpoints corresponding to the edge's input/output) is deferred
    // until later
    void deferEdge(QString srcNode, QString srcOutput, QString tgtNode, QString tgtInput, QQuickItem *creator);
    // Rename a node in the graph
    bool renameNode(QString currentName, QString newName);
    // Select an existing edge for deletion, determined from the target node and input parameter name
    void deleteEdgeFromTarget(QString tgtNode, QString tgtInput);
    // Select all relevant edges for deletion, determined from the source node and output parameter name
    void deleteEdgeFromSource(QString sourceNode, QString sourceInput);
    // Provide relative coordinates for an input on a node
    void addInput(QString nodeName, QString paramName, double x, double y);
    // Provide relative coordinates for an output on a node
    void addOutput(QString nodeName, QString paramName, double x, double y);
    // Add a new node at a specific position
    void emplace_back(int x, int y, QVariant type, QString name, bool avoidSamePosition = false);
    // Switch to parent graph
    void upLevel();
    // Move into an inner graph
    void descend(int index);
    /* UNUSED
    // public wrapper of connect_
    bool connect(std::string source, int sourceIndex, std::string destination, int destinationIndex);
    // Public wrapper of disconnect_
    bool disconnect(std::string source, int sourceIndex, std::string destination, int destinationIndex);
    */
};
