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
#include <stdexcept>

GraphModel::GraphModel() : nodes_(this), graph_(nullptr), edges_(this, graph_)
{
    QObject::connect(&nodes_, &GraphNodeModel::updatePosition, &edges_, &GraphEdgeModel::updatePosition);
}

Graph *GraphModel::graph() { return graph_; }

void GraphModel::setGraph(Graph *graph)
{
    // If the new graph already contains edges, we will need to reconstruct each node's edge connections in the UI.
    // Therefore the connected nodes are added to a queue of 'reconstructibles' whose existing edges will be re-rendered.
    if (!graph->edges().empty())
    {
        reconstructibleNodes_.emplace();
        auto &existingNodes = graph->nodes();
        for (const auto &[name, _] : existingNodes)
            reconstructibleNodes_->push_back(name);
    }

    graph_ = graph;
    parameterEndPoints_.clear();
    nodes_.reset();
    edges_.reset();
    Q_EMIT graphChanged();
}

// Return the parameter endpoints model for a given graph
ParameterEndPointsModel *GraphModel::parameterEndPoints() { return &parameterEndPoints_; }

// Access the GraphNodeModel
GraphNodeModel *GraphModel::nodes() { return &nodes_; }

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
void GraphModel::addInput(QString nodeName, QString paramName, double x, double y)
{
    auto nodeIndex =
        std::distance(wrapped_.begin(), std::find_if(wrapped_.begin(), wrapped_.end(), [&](const auto &wrappedNode)
                                                     { return wrappedNode.rawValue().name() == nodeName.toStdString(); }));
    auto &node = wrapped_[nodeIndex];
    x += 16;
    y += 64;
    node.inputsPos.insert({paramName.toStdString(), {x, y}});
}

// Provide relative coordinates for an output on a node
void GraphModel::addOutput(QString nodeName, QString paramName, double x, double y)
{
    auto nodeIndex =
        std::distance(wrapped_.begin(), std::find_if(wrapped_.begin(), wrapped_.end(), [&](const auto &wrappedNode)
                                                     { return wrappedNode.rawValue().name() == nodeName.toStdString(); }));
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
    Q_EMIT graphChanged();
}

// Return graph canvas dimensions
QSizeF GraphModel::canvasDimensions() const { return canvasDimensions_; }

// Set graph canvas dimensions
void GraphModel::setCanvasDimensions(const QSizeF &canvasDimensions)
{
    canvasDimensions_ = canvasDimensions;
    Q_EMIT canvasDimensionsChanged();
}

void GraphModel::emplace_back(int x, int y, QVariant type, QString name, bool avoidSamePosition)
{
    if (!graph_)
        Messenger::exception(
            "GraphModel has no graph.  This should have been impossible.  Please let the Dissolve developers know about this.");
    nodes_.beginInsertRows({}, graph_->nodes().size(), graph_->nodes().size() + 1);
    auto nodeType = type.toString().toStdString();
    auto node = graph_->createNode(nodeType, name.toStdString());
    auto dX = 0, dY = 0;
    if (avoidSamePosition)
        findUniqueXY(x, y, dX, dY);
    node->x = x + dX;
    node->y = y + dY;
    auto &item = wrapped_.emplace_back(*node);
    item.rawValue().setName(name.toStdString());
    nodes_.endInsertRows();
    Q_EMIT graphChanged();
}

void GraphModel::deleteNode(int idx)
{
    nodes_.beginRemoveRows({}, idx, idx);
    const auto nodeType = wrapped_[idx].rawValue().type();
    std::string nodeName{wrapped_[idx].rawValue().name()};

    // Remove any endpoints corresponding to this node
    if (curveInputEndPoints_.contains(&wrapped_[idx].rawValue()))
        curveInputEndPoints_.erase(&wrapped_[idx].rawValue());
    if (curveOutputEndPoints_.contains(&wrapped_[idx].rawValue()))
        curveOutputEndPoints_.erase(&wrapped_[idx].rawValue());
    parameterEndPoints()->remove(&wrapped_[idx].rawValue());

    // Erase the wrapped node
    wrapped_.erase(wrapped_.begin() + idx);

    // Delete the edges corresponding to this node
    edges_.removeConnected(nodeName);

    // Erase the underlying graph node
    graph_->reverseNodes().erase(graph_->findNode(nodeName));
    graph_->nodes().erase(nodeName);

    nodes_.endRemoveRows();

    Q_EMIT graphChanged();
    Q_EMIT decrementNodeTypeRequired(std::string(nodeType));
}

GraphEdgeModel *GraphModel::edges() { return &edges_; }

int GraphModel::nEdges()
{
    if (graph_ == nullptr)
        return 0;
    return edges_.rowCount();
}

// Select a specific output for connection
void GraphModel::addEdge(QString srcNode, QString srcOutput, QString tgtNode, QString tgtInput)
{
    EdgeDefinition edge(srcNode.toStdString(), srcOutput.toStdString(), tgtNode.toStdString(), tgtInput.toStdString());
    if (edges_.add(edge))
        addEndPoints(srcNode.toStdString(), srcOutput.toStdString(), tgtNode.toStdString(), tgtInput.toStdString());
}

// Adds a new edge, but the connection (addition of QML endpoints corresponding to the edge's input/output) is deferred until
// later
void GraphModel::deferEdge(QString srcNode, QString srcOutput, QString tgtNode, QString tgtInput, QQuickItem *creator)
{
    EdgeDefinition edge(srcNode.toStdString(), srcOutput.toStdString(), tgtNode.toStdString(), tgtInput.toStdString());
    if (edges_.add(edge))
    {
        auto parentNode = creator->property("parentNodeBox").value<QObject *>();
        auto parentNodeName = parentNode->property("nodeName").toString().toStdString();
        auto nodeIt = std::find_if(wrapped_.begin(), wrapped_.end(), [&](const NodeWrapper &wrappedNode)
                                   { return wrappedNode.rawValue().name() == parentNodeName; });
        auto &creatorNode = wrapped_[std::distance(wrapped_.begin(), nodeIt)];
        if (creator->property("connectionType").value<int>() == 1)
            return creatorNode.inputs->resetParameters();
        if (creator->property("connectionType").value<int>() == 0)
            return creatorNode.outputs->resetParameters();
    }
}

// Rename a node in the graph
bool GraphModel::renameNode(QString currentName, QString newName)
{
    auto nodeIt = std::find_if(wrapped_.begin(), wrapped_.end(), [&](const auto &wrappedNode)
                               { return wrappedNode.rawValue().name() == currentName.toStdString(); });

    if (nodeIt != wrapped_.end() && !(graph_->findNode(newName.toStdString())))
        nodes_.setData(nodes_.index(std::distance(wrapped_.begin(), nodeIt)), QVariant::fromValue(newName),
                       Qt::UserRole + GraphNodeModel::NAME);
    else
        return false;
    return true;
}

// Select an existing edge for deletion, determined from the target node and input parameter name
void GraphModel::deleteEdgeFromTarget(QString tgtNode, QString tgtInput)
{
    auto edge = graph_->findEdgeByTarget(tgtNode.toStdString(), tgtInput.toStdString());
    auto sourceNode = std::string(edge->sourceNode().name());
    auto targetNode = std::string(edge->targetNode().name());
    auto sourceOutput = std::string(edge->sourceOutput().name());
    auto targetInput = std::string(edge->targetInput().name());
    if (edges_.remove(*edge))
    {
        auto removed = parameterEndPoints()->remove(sourceNode, sourceOutput, targetNode, targetInput);
        for (auto &[_, target] : removed)
            target->setProperty("locked", false);
    }
}

// Select all relevant edges for deletion, determined from the source node and output parameter name
void GraphModel::deleteEdgeFromSource(QString sourceNode, QString sourceInput)
{
    auto edges = graph_->findEdgesBySource(sourceNode.toStdString(), sourceInput.toStdString());
    for (const auto &edge : edges)
    {
        auto sourceNode = std::string(edge->sourceNode().name());
        auto targetNode = std::string(edge->targetNode().name());
        auto sourceOutput = std::string(edge->sourceOutput().name());
        auto targetInput = std::string(edge->targetInput().name());
        if (edges_.remove(*edge))
            parameterEndPoints()->remove(sourceNode, sourceOutput, targetNode, targetInput);
    }
}

/* UNUSED
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
*/

// Return bool - true if node exists in graph
bool GraphModel::isValidNode(QVariant nodeName) const { return graph_->findNode(nodeName.toString().toStdString()); }

// Return bool - true if graph run was successful/unchanged, false if failed
void GraphModel::run(QVariant nodeName)
{
    auto name = nodeName.toString().toStdString();
    auto node = graph_->findNode(name);
    Q_EMIT graphRunComplete(node->run(), name);
}

int GraphModel::indexByName(std::string_view name)
{
    // FIXME
    return 0;
}

//
void GraphModel::reload() { setGraph(graph_); }

//
void GraphModel::resetEndPoints()
{
    parameterEndPoints_.resetFromEdges(graph_->edges(), curveOutputEndPoints_, curveInputEndPoints_);
}

// Replace the target DropArea, for instance when the existing underlying QQuickItem * is no longer valid
void GraphModel::replaceTargetEndPoint(QString nodeName, QString paramName, QQuickItem *newDropArea)
{
    auto endPoints = parameterEndPoints_.endPoints();
    // If no endpoints present, don't do anything
    if (endPoints.empty())
        return;

    // Find the new DropArea in the input endpoints, inorder to replace the current target endpoint with it
    auto replaceIt = std::find_if(endPoints.begin(), endPoints.end(),
                                  [&](const std::pair<QQuickItem *, QQuickItem *> &pair)
                                  {
                                      auto &target = pair.second;
                                      auto targetNodeName = target->property("nodeName").toString();
                                      auto targetParam = target->property("paramName").toString();
                                      return targetNodeName == nodeName && targetParam == paramName;
                                  });
    parameterEndPoints_.replaceTarget(std::distance(endPoints.begin(), replaceIt), newDropArea);
}

// Replace the target DropArea, for instance when the existing underlying QQuickItem * is no longer valid
void GraphModel::replaceSourceEndPoint(QString nodeName, QString paramName, QQuickItem *newDropArea)
{
    auto endPoints = parameterEndPoints_.endPoints();
    // If no endpoints present, don't do anything
    if (endPoints.empty())
        return;

    // Find the new DropArea in the input endpoints, inorder to replace the current target endpoint with it
    auto replaceIt = std::find_if(endPoints.begin(), endPoints.end(),
                                  [&](const std::pair<QQuickItem *, QQuickItem *> &pair)
                                  {
                                      auto &source = pair.first;
                                      auto sourceNodeName = source->property("nodeName").toString();
                                      auto sourceParam = source->property("paramName").toString();
                                      return sourceNodeName == nodeName && sourceParam == paramName;
                                  });
    parameterEndPoints_.replaceSource(std::distance(endPoints.begin(), replaceIt), newDropArea);
}

//
bool GraphModel::renameInput(QString nodeName, QString currentName, QString newName)
{
    auto node = graph_->findNode(nodeName.toStdString());
    auto focusNodeIsGraph = dynamic_cast<Graph *>(node);
    auto focusNodeIsOutputs = dynamic_cast<OutputsNode *>(node);

    // If the node owning the input is not of type Graph or Outputs, input rename is not permitted
    if (!(focusNodeIsGraph || focusNodeIsOutputs))
        return false;

    auto it = std::find_if(wrapped_.begin(), wrapped_.end(),
                           [&](const auto &wrappedNode) { return wrappedNode.rawValue().name() == nodeName.toStdString(); });
    auto focusNodeIdx = std::distance(wrapped_.begin(), it);
    auto &focusNode = wrapped_[focusNodeIdx];

    // New name cannot be already in use within this node's own inputs
    if (focusNode.inputs->values().contains(newName.toStdString()))
        return false;

    // Rename the underlying parameter
    if (!focusNode.inputs->values().at(currentName.toStdString())->setName(newName.toStdString()))
        return false;

    // Remove the current name from the input endpoints map
    curveInputEndPoints_.find(node)->second.erase(currentName.toStdString());

    // Rename the input entry within the node parameter map
    focusNode.inputs->rename(currentName.toStdString(), newName.toStdString());

    if (focusNodeIsGraph)
    {
        /*
         * Propagate change - due to renaming the graph's input - to the the graph's inputs node
         */

        auto graph = dynamic_cast<Graph *>(node);
        auto inputsNode = graph->findNode("Inputs");
        auto model = ParameterModel(inputsNode->outputs());

        // Rename the underlying parameter
        if (!model.values().at(currentName.toStdString())->setName(newName.toStdString()))
            return false;

        // Remove the current name from the output endpoints map (if the graph's inputs have been mapped yet)
        if (curveOutputEndPoints_.find(inputsNode) != curveOutputEndPoints_.end())
            curveOutputEndPoints_[inputsNode].erase(currentName.toStdString());

        // Rename the input entry within the node parameter map
        model.rename(currentName.toStdString(), newName.toStdString());

        return true;
    }
    else if (focusNodeIsOutputs)
    {
        /*
         * Propagate change - due to renaming an outputs node input - to the parent graph (node) outputs
         */

        auto graph = node->parentGraph();
        auto model = ParameterModel(graph->outputs());

        // Rename the underlying parameter
        if (!model.values().at(currentName.toStdString())->setName(newName.toStdString()))
            return false;

        // Remove the current name from the input endpoints map
        if (curveInputEndPoints_.find(graph) != curveInputEndPoints_.end())
            curveInputEndPoints_[graph].erase(currentName.toStdString());

        // Rename the input entry within the node parameter map
        model.rename(currentName.toStdString(), newName.toStdString());

        return true;
    }

    return false;
}

//
bool GraphModel::renameOutput(QString nodeName, QString currentName, QString newName)
{
    auto node = graph_->findNode(nodeName.toStdString());
    auto focusNodeIsGraph = dynamic_cast<Graph *>(node);
    auto focusNodeIsInputs = dynamic_cast<InputsNode *>(node);

    // If the node owning the output is not of type Graph or Outputs, output rename is not permitted
    if (!(focusNodeIsGraph || focusNodeIsInputs))
        return false;

    auto it = std::find_if(wrapped_.begin(), wrapped_.end(),
                           [&](const auto &wrappedNode) { return wrappedNode.rawValue().name() == nodeName.toStdString(); });
    auto focusNodeIdx = std::distance(wrapped_.begin(), it);
    auto &focusNode = wrapped_[focusNodeIdx];

    // New name cannot be already in use within this node's own outputs
    if (focusNode.outputs->values().contains(newName.toStdString()))
        return false;

    if (!focusNode.outputs->values().at(currentName.toStdString())->setName(newName.toStdString()))
        return false;

    // Remove the current name from the output endpoints map
    curveOutputEndPoints_.find(node)->second.erase(currentName.toStdString());

    // Rename the output entry within the node parameter map
    focusNode.outputs->rename(currentName.toStdString(), newName.toStdString());

    if (focusNodeIsGraph)
    {
        /*
         * Propagate change - due to renaming the graph's output - to the the graph's outputs node
         */

        auto graph = dynamic_cast<Graph *>(node);
        auto outputsNode = graph->findNode("Outputs");
        auto model = ParameterModel(outputsNode->inputs());

        // Rename the underlying parameter
        if (!model.values().at(currentName.toStdString())->setName(newName.toStdString()))
            return false;

        // Remove the current name from the input endpoints map (if the graph's outputs have been mapped yet)
        if (curveInputEndPoints_.find(outputsNode) != curveInputEndPoints_.end())
            curveInputEndPoints_[outputsNode].erase(currentName.toStdString());

        // Rename the input entry within the node parameter map
        model.rename(currentName.toStdString(), newName.toStdString());

        return true;
    }
    else if (focusNodeIsInputs)
    {
        /*
         * Propagate change - due to renaming an inputs node output - to the parent graph (node) inputs
         */

        auto graph = node->parentGraph();
        auto model = ParameterModel(graph->inputs());

        // Rename the underlying parameter
        if (!model.values().at(currentName.toStdString())->setName(newName.toStdString()))
            return false;

        // Remove the current name from the input endpoints map
        if (curveOutputEndPoints_.find(graph) != curveInputEndPoints_.end())
            curveOutputEndPoints_[graph].erase(currentName.toStdString());

        // Rename the input entry within the node parameter map
        model.rename(currentName.toStdString(), newName.toStdString());

        return true;
    }

    return false;
}

// Returns bool - true if we are currently reconstructing existing nodes in the current graph
bool GraphModel::nodeReconstructionInProgress() { return reconstructibleNodes_.has_value() && !reconstructibleNodes_->empty(); }

// Record that a node with a given name has been reconstructed
void GraphModel::reconstructed(QString constructedName)
{
    bool updateEndPoints = false;
    if (nodeReconstructionInProgress())
    {
        updateEndPoints = reconstructibleNodes_->size() == 1;
        auto removeIt =
            std::find_if(reconstructibleNodes_->begin(), reconstructibleNodes_->end(),
                         [&constructedName](const auto &otherName) { return constructedName.toStdString() == otherName; });
        reconstructibleNodes_->erase(removeIt);

        if (updateEndPoints)
        {
            reconstructibleNodes_.reset();
            parameterEndPoints_.resetFromEdges(graph_->edges(), curveOutputEndPoints_, curveInputEndPoints_);
        }
    }
}

// Returns bool - true if the node's parameter is connected to anything
bool GraphModel::hasConnections(QString nodeName, QString paramName)
{
    if (graph_->findNode(nodeName.toStdString())->findInput(paramName.toStdString()))
        return graph_->findEdgeByTarget(nodeName.toStdString(), paramName.toStdString()) != nullptr;
    else if (graph_->findNode(nodeName.toStdString())->findOutput(paramName.toStdString()))
        return !graph_->findEdgesBySource(nodeName.toStdString(), paramName.toStdString()).empty();
    else
        return false;
}

// Add endpoints between the source node and its output to a target node and its input, where the edge connection has previously
// been deferred
void GraphModel::addDeferredEndPoints()
{
    const auto &edges = graph_->edges();
    if (edges.empty())
        return;

    const auto edge = edges.back().get();
    addEndPoints(std::string(edge->sourceNode().name()), std::string(edge->sourceOutput().name()),
                 std::string(edge->targetNode().name()), std::string(edge->targetInput().name()));
}

// Map an available input endpoint for a node parameter to an input QML DropArea
void GraphModel::mapInputEndPoint(QVariant nodeName, QVariant paramName, QQuickItem *endPoint)
{
    auto name = nodeName.toString().toStdString();
    auto param = paramName.toString().toStdString();
    auto node = graph_->findNode(name);
    if (!curveInputEndPoints_.contains(node))
        curveInputEndPoints_.emplace(node, std::map<std::string, QQuickItem *>{});
    curveInputEndPoints_[node].emplace(paramName.toString().toStdString(), endPoint);
}

// Map an available output endpoint for a node parameter to an output QML DropArea
void GraphModel::mapOutputEndPoint(QVariant nodeName, QVariant paramName, QQuickItem *endPoint)
{
    auto name = nodeName.toString().toStdString();
    auto param = paramName.toString().toStdString();
    auto node = graph_->findNode(name);
    if (!curveOutputEndPoints_.contains(node))
        curveOutputEndPoints_.emplace(node, std::map<std::string, QQuickItem *>{});
    curveOutputEndPoints_[node].emplace(param, endPoint);
}

void GraphModel::addEndPoints(std::string sourceNodeName, std::string sourceParamName, std::string targetNodeName,
                              std::string targetParamName)
{
    auto sourceNode = graph_->findNode(sourceNodeName);
    auto targetNode = graph_->findNode(targetNodeName);
    auto sourceDropArea = curveOutputEndPoints_[sourceNode][sourceParamName];
    auto targetDropArea = curveInputEndPoints_[targetNode][targetParamName];
    parameterEndPoints()->add(sourceDropArea, targetDropArea);
}

// Find a unique point in the graph's x-y space for positioning when instantiated
void GraphModel::findUniqueXY(int x, int y, int &dX, int &dY)
{
    const int maxX = canvasDimensions_.width();
    const int maxY = canvasDimensions_.height();
    const auto border = 100;
    std::set<std::pair<int, int>> occupied;
    std::ranges::transform(wrapped_, std::inserter(occupied, occupied.end()),
                           [](const auto &wrappedNode)
                           {
                               auto &val = wrappedNode.rawValue();
                               return std::pair{val.x, val.y};
                           });

    const int displacement = 500;
    std::uniform_int_distribution<int> dist(-displacement, displacement);
    bool isOccupied = true;
    while (isOccupied)
    {
        dX = dist(rnG_);
        dY = dist(rnG_);

        // If we are outside the graph view's border area, continue
        if ((x + dX < border || x + dX > (maxX - border)) || (y + dY < border || y + dY > (maxY - border)))
            continue;

        if (!occupied.contains({x + dX, y + dY}))
            isOccupied = false;
    }
}

void GraphModel::handleReset() { Q_EMIT(Q_EMIT graphChanged()); }
