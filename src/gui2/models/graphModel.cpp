// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "graphModel.h"
#include "graphEdgeModel.h"
#include "graphNodeModel.h"
#include "nodes/edge.h"
#include "nodes/inputs.h"
#include "nodes/outputs.h"
#include <QAbstractItemModel>
#include <QVariant>
#include <algorithm>
#include <iostream>
#include <ranges>
#include <set>

GraphModel::GraphModel() : nodes_(this), graph_(nullptr), edges_(this, graph_)
{
    // TODO: Using the current edge management method (by rendering edges between the positions of QML objects corresponding to
    // drag/drop locations) this connection has no effect. This model's exposed roles sourceX, sourceY, targetX, and targetY,
    // are not used. We should consider a refactor to remove this unused code, and potentially relegate the GraphEdgeModel to a
    // QObject derivate, or even a simple struct interface for edges.
    //
    // QObject::connect(&nodes_, &GraphNodeModel::updatePosition, &edges_, &GraphEdgeModel::updatePosition);
}

// Return the graph status
const std::optional<NodeConstants::ProcessResult> &GraphModel::graphStatus() const { return graphStatus_; }

//
bool GraphModel::graphControlsEnabled() { return graphProgressComplete_; }

// Returns a lambda to assign a default position to nodes of type input/output/loopbacks
std::function<std::optional<double>(Node *)> &GraphModel::nodeXPositionInitialiser() { return nodeXPositionInitialiser_; }

// Returns a lambda to assign a default position to nodes of type input/output/loopbacks
std::function<std::optional<double>(Node *)> &GraphModel::nodeYPositionInitialiser() { return nodeYPositionInitialiser_; }

// Set the graph status
void GraphModel::setGraphStatus(NodeConstants::ProcessResult status)
{
    if (graphStatus_.has_value())
        graphStatus_.reset();
    graphStatus_.emplace(status);
}

Graph *GraphModel::graph() { return graph_; }

// Return the ParameterEndPointModel
ParameterEndPointsModel *GraphModel::parameterEndPoints() { return &endPointsModel_; }

void GraphModel::setGraph(Graph *graph)
{
    graph_ = graph;

    nodes_.beginResetModel();
    wrapped_.clear();
    int idx = 0;
    for (auto &[name, node] : graph->nodes())
        auto &item = wrapped_.emplace_back(*node);
    nodes_.endResetModel();

    nodes_.updateGraph();
    edges_.reset();
    graphChanged();
}

// Access the GraphNodeModel
GraphNodeModel *GraphModel::nodes() { return &nodes_; }

// Returns the graph status icon
QUrl GraphModel::statusIcon()
{
    if (!graphProgressComplete_)
        return QUrl("qrc:/DissolveIconsModule/waiting.svg");
    if (!graphStatus_.has_value())
        return QUrl("qrc:/DissolveIconsModule/unknown.svg");
    if (*graphStatus_ == NodeConstants::ProcessResult::Unchanged || *graphStatus_ == NodeConstants::ProcessResult::Success)
        return QUrl("qrc:/DissolveIconsModule/true.svg");
    return QUrl("qrc:/DissolveIconsModule/false.svg");
}

int GraphModel::count() { return nodes_.rowCount(); }

QString GraphModel::location() const
{
    if (!graph_)
        return "";
    return QString::fromStdString(graph_->location());
};

bool GraphModel::atRoot() const
{
    if (!graph_)
        return true;
    return !graph_->parentGraph();
}

// Provide relative coordinates for an input on a node
void GraphModel::addInput(int nodeIndex, QString paramName, double x, double y)
{
    auto &node = wrapped_[nodeIndex];
    x += 16;
    y += 64;
    node.inputsPos.insert({paramName.toStdString(), {x, y}});
}

// Provide relative coordinates for an output on a node
void GraphModel::addOutput(int nodeIndex, QString paramName, double x, double y)
{
    auto &node = wrapped_[nodeIndex];
    x += 16;
    y += 64;
    node.outputPos.insert({paramName.toStdString(), {x, y}});
}

// Switch to parent graph
void GraphModel::upLevel()
{
    if (!graph_)
        return;
    setGraph(graph_->parentGraph());
}

// Move into an inner graph
void GraphModel::descend(int index)
{
    auto &node = wrapped_[index];
    if (node.hasInner())
    {
        setGraph(static_cast<Graph *>(&node.rawValue()));
    }
}

// Add a pre-created node
void GraphModel::addNode(std::unique_ptr<Node> node, std::string_view name)
{
    nodes_.beginInsertRows({}, graph_->nodes().size(), graph_->nodes().size() + 1);
    graph_->addNode(std::move(node), name);
    wrapped_.emplace_back(*graph_->nodes()[std::string(name)]);
    nodes_.endInsertRows();
    graphChanged();
}

// Return graph canvas dimensions
QSizeF GraphModel::canvasDimensions() const { return canvasDimensions_; }

// Set graph canvas dimensions
void GraphModel::setCanvasDimensions(const QSizeF &canvasDimensions)
{
    canvasDimensions_ = canvasDimensions;
    Q_EMIT canvasDimensionsChanged();
}

void GraphModel::emplace_back(int x, int y, QVariant type, std::string name)
{
    if (!graph_)
        Messenger::exception(
            "GraphModel has no graph.  This should have been impossible.  Please let the Dissolve developers know about this.");
    nodes_.beginInsertRows({}, graph_->nodes().size(), graph_->nodes().size() + 1);
    auto nodeType = type.toString().toStdString();
    auto node = graph_->createNode(nodeType, name);
    node->x = x;
    node->y = y;
    auto &item = wrapped_.emplace_back(*node);
    item.rawValue().setName(name);
    nodes_.endInsertRows();
    graphChanged();
}

void GraphModel::deleteNode(int idx)
{
    nodes_.beginRemoveRows({}, idx, idx);
    const auto nodeType = wrapped_[idx].rawValue().type();
    std::string nodeName{wrapped_[idx].rawValue().name()};
    if (inputEndPoints_.contains(&wrapped_[idx].rawValue()))
        inputEndPoints_.erase(&wrapped_[idx].rawValue());
    if (outputEndPoints_.contains(&wrapped_[idx].rawValue()))
        outputEndPoints_.erase(&wrapped_[idx].rawValue());
    endPointsModel_.remove(&wrapped_[idx].rawValue());
    wrapped_.erase(wrapped_.begin() + idx);

    edges_.deleteByNode(nodeName);

    graph_->reverseNodes().erase(graph_->findNode(nodeName));
    graph_->nodes().erase(nodeName);
    nodes_.endRemoveRows();

    graphChanged();
    decrementNodeTypeRequired(std::string(nodeType));
}

GraphEdgeModel *GraphModel::edges() { return &edges_; }

int GraphModel::nEdges()
{
    if (graph_ == nullptr)
        return 0;
    return edges_.rowCount();
}

void GraphModel::addEdge(QString srcNode, QString srcOutput, QString tgtNode, QString tgtInput)
{
    EdgeDefinition edge(srcNode.toStdString(), srcOutput.toStdString(), tgtNode.toStdString(), tgtInput.toStdString());
    edges_.addEdge(edge);
    addEndPoints(srcNode.toStdString(), srcOutput.toStdString(), tgtNode.toStdString(), tgtInput.toStdString());
}

// public wrapper of connect_
bool GraphModel::connect(std::string source, int sourceIndex, std::string destination, int destinationIndex)
{
    // FIXME
    return false;
}
// Public wrapper of disconnect_
bool GraphModel::disconnect(std::string source, int sourceIndex, std::string destination, int destinationIndex)
{
    // FIXME
    return false;
}

// Return bool - true if node exists in graph
bool GraphModel::isValidNode(QVariant nodeName) const { return graph_->findNode(nodeName.toString().toStdString()); }

// Return bool - true if graph run was successful/unchanged, false if failed
void GraphModel::run(QVariant nodeName)
{
    auto name = nodeName.toString().toStdString();
    auto node = graph_->findNode(name);
    auto result = std::make_shared<NodeConstants::ProcessResult>();
    auto *dissolveThread = QThread::create([this, node, result] { *result = node->run(); });
    QObject::connect(dissolveThread, &QThread::started, this, [this]() { Q_EMIT graphRunStarted(); });
    QObject::connect(this, &GraphModel::graphRunStarted, this,
                     [this]()
                     {
                         graphProgressComplete_ = false;

                         // Graph progress changed (started), so update controls enabled
                         Q_EMIT graphProgressChanged();
                     });
    QObject::connect(dissolveThread, &QThread::finished, this,
                     [this, node, result]()
                     {
                         if (!result)
                         {
                             Q_EMIT graphRunComplete(NodeConstants::ProcessResult::Failed, std::string(node->name()));
                             return;
                         }
                         setGraphStatus(*result.get());

                         // Update dynamic outputs
                         auto dynamicNodes = nodes_.findAllByRoleTrue(GraphNodeModel::HAS_DYNAMIC_OUTPUTS + Qt::UserRole);
                         for (auto &nodeWrapper : dynamicNodes)
                             nodeWrapper->outputs->resetParameters();

                         Q_EMIT graphRunComplete(*result.get(), std::string(node->name()));
                     });
    QObject::connect(this, &GraphModel::graphRunComplete, this,
                     [this]()
                     {
                         graphProgressComplete_ = true;

                         // Graph progress changed (finished), so update controls enabled
                         Q_EMIT graphProgressChanged();
                     });
    QObject::connect(dissolveThread, &QThread::finished, dissolveThread, &QObject::deleteLater);
    dissolveThread->start();
}

int GraphModel::indexByName(std::string_view name)
{
    // FIXME
    return 0;
}

void GraphModel::initialiseInputEndPoints(QVariant nodeName, QVariant paramName, QQuickItem *endPoint)
{
    auto name = nodeName.toString().toStdString();
    auto param = paramName.toString().toStdString();
    auto node = graph_->findNode(name);
    if (!inputEndPoints_.contains(node))
        inputEndPoints_.emplace(node, std::map<std::string, QQuickItem *>{});
    inputEndPoints_[node].emplace(paramName.toString().toStdString(), endPoint);
}

void GraphModel::initialiseOutputEndPoints(QVariant nodeName, QVariant paramName, QQuickItem *endPoint)
{
    auto name = nodeName.toString().toStdString();
    auto param = paramName.toString().toStdString();
    auto node = graph_->findNode(name);
    if (!outputEndPoints_.contains(node))
        outputEndPoints_.emplace(node, std::map<std::string, QQuickItem *>{});
    outputEndPoints_[node].emplace(param, endPoint);
}

void GraphModel::addEndPoints(std::string sourceNodeName, std::string sourceParamName, std::string targetNodeName,
                              std::string targetParamName)
{
    auto sourceNode = graph_->findNode(sourceNodeName);
    auto targetNode = graph_->findNode(targetNodeName);
    endPointsModel_.add(outputEndPoints_[sourceNode][sourceParamName], inputEndPoints_[targetNode][targetParamName]);
}

void GraphModel::handleReset() { Q_EMIT(graphChanged()); }

void ParameterEndPointsModel::add(QQuickItem *sourceDropArea, QQuickItem *targetDropArea)
{
    auto row = endPoints_.size();
    beginInsertRows(QModelIndex(), row, row);
    endPoints_.push_back({sourceDropArea, targetDropArea});
    endInsertRows();
}

void ParameterEndPointsModel::remove(const Node *node)
{
    auto shouldRemove = [&](int i) -> bool
    {
        auto &[sourceDropArea, targetDropArea] = endPoints_[i];
        auto sourceParentNode = sourceDropArea->property("parentNodeBox").value<QObject *>();
        auto sourceParentNodeName = sourceParentNode->property("nodeName").toString().toStdString();
        auto targetParentNode = targetDropArea->property("parentNodeBox").value<QObject *>();
        auto targetParentNodeName = targetParentNode->property("nodeName").toString().toStdString();
        return sourceParentNodeName == node->name() || targetParentNodeName == node->name();
    };

    for (int row = endPoints_.size() - 1; row >= 0; --row)
    {
        if (shouldRemove(row))
        {
            beginRemoveRows(QModelIndex(), row, row);
            endPoints_.erase(endPoints_.begin() + row);
            endRemoveRows();
        }
    }
}

int ParameterEndPointsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return endPoints_.size();
}

QVariant ParameterEndPointsModel::data(const QModelIndex &index, int role) const
{
    auto &[source, target] = endPoints_[index.row()];
    switch (role)
    {
        case EndPointDisplayRoles::Source:
            return QVariant::fromValue(source);
        case EndPointDisplayRoles::Target:
            return QVariant::fromValue(target);
        default:
            return QVariant();
    }
}

Qt::ItemFlags ParameterEndPointsModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant ParameterEndPointsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "SourceDropArea";
            case 1:
                return "TargetDropArea";
        }

    return {};
}

QHash<int, QByteArray> ParameterEndPointsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Source] = "sourceDropArea";
    roles[Target] = "targetDropArea";
    return roles;
}