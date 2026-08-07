// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "gui2/models/graphNodeModel.h"
#include "gui2/models/nodeWrapper.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include <QAbstractListModel>
#include <QQuickItem>
#include <qtmetamacros.h>

class GraphNodeModel;
class GraphEdgeModel;

class ParameterEndPointsModel : public QAbstractListModel
{
    friend GraphModel;

    Q_OBJECT

    public:
    using ParameterEndPoints = std::vector<std::pair<QQuickItem *, QQuickItem *>>;
    ParameterEndPointsModel() = default;

    enum EndPointDisplayRoles
    {
        Source = Qt::DisplayRole,
        Target = Qt::UserRole + 1,
    };

    // Add a pair of DropArea QQuickItem *, representing the endpoints
    void add(QQuickItem *sourceDropArea, QQuickItem *targetDropArea);
    // Remove a pair of DropArea QQuickItem *, based on the parent node that has been deleted
    void remove(const Node *node);

    protected:
    // Vector of parameter endpoint QQuickItem * pairs
    ParameterEndPoints endPoints_;

    /*
     * QAbstractListModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

// This is the base class for any node graph type
class GraphModel : public QObject
{
    using NodeParameterEndPointsMap = std::map<const Node *, std::map<std::string, QQuickItem *>>;

    Q_OBJECT;
    Q_PROPERTY(Graph *graph READ graph WRITE setGraph NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);
    Q_PROPERTY(QAbstractListModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(ParameterEndPointsModel *parameterEndPoints READ parameterEndPoints CONSTANT);
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

    // Model for the edge end points corresponding to parameter drop areas
    ParameterEndPointsModel *parameterEndPoints();
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
    // Map of node parameters to endpoint QQuickItem pointers within GraphView
    NodeParameterEndPointsMap inputEndPoints_;
    NodeParameterEndPointsMap outputEndPoints_;
    ParameterEndPointsModel endPointsModel_;
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
    Q_INVOKABLE void initialiseInputEndPoints(QVariant nodeName, QVariant paramName, QQuickItem *endPoint);
    Q_INVOKABLE void initialiseOutputEndPoints(QVariant nodeName, QVariant paramName, QQuickItem *endPoint);
    // Return bool - true if node exists in graph
    Q_INVOKABLE bool isValidNode(QVariant nodeName) const;
    // Return bool - true if graph run was successful/unchanged, false if failed
    Q_INVOKABLE void run(QVariant nodeName);

    private:
    void addEndPoints(std::string sourceNodeName, std::string sourceParamName, std::string targetNodeName,
                      std::string targetParamName);

    private:
    // Check whether a given source and destination can be connected
    bool isValidEdgeSource_(Edge &edge);
    // Connect two nodes
    bool connect_(Edge &edge);
    // Remove a connection
    bool disconnect_(Edge &edge);

    Q_SIGNALS:
    void graphChanged();
    void decrementNodeTypeRequired(const std::string &);
    void graphRunComplete(NodeConstants::ProcessResult status, std::string node);

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
    void emplace_back(int x, int y, QVariant type, std::string name);

    // Switch to parent graph
    void upLevel();
    // Move into an inner graph
    void descend(int index);
};
