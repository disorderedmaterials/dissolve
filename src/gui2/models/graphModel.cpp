// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "graphModel.h"
#include "graphEdgeModel.h"
#include "graphNodeModel.h"
#include "nodes/edge.h"
#include <QAbstractItemModel>
#include <QVariant>
#include <iostream>

GraphModel::GraphModel() : nodes_(this), graph_(nullptr), edges_(this, graph_)
{
    QObject::connect(&nodes_, &GraphNodeModel::updatePosition, &edges_, &GraphEdgeModel::updatePosition);
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
QAbstractListModel *GraphModel::nodes() { return &nodes_; }

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

void GraphModel::emplace_back(int x, int y, QVariant type, QVariant name)
{
    if (!graph_)
        Messenger::exception(
            "GraphModel has no graph.  This should have been impossible.  Please let the Dissolve developers know about this.");
    nodes_.beginInsertRows({}, graph_->nodes().size(), graph_->nodes().size() + 1);
    auto nodeType = type.toString().toStdString();
    auto nodeName = name.toString().toStdString();
    auto node = graph_->createNode(nodeType, nodeName);
    node->x = x;
    node->y = y;
    auto &item = wrapped_.emplace_back(*node);
    item.rawValue().setName(name.toString().toStdString());
    nodes_.endInsertRows();
    graphChanged();
}

void GraphModel::deleteNode(int idx)
{
    nodes_.beginRemoveRows({}, idx, idx);
    std::string index{wrapped_[idx].rawValue().name()};
    if (inputEndPoints_.contains(&wrapped_[idx].rawValue()))
        inputEndPoints_.erase(&wrapped_[idx].rawValue());
    if (outputEndPoints_.contains(&wrapped_[idx].rawValue()))
        outputEndPoints_.erase(&wrapped_[idx].rawValue());
    endPointsModel_.remove(&wrapped_[idx].rawValue());
    wrapped_.erase(wrapped_.begin() + idx);

    edges_.deleteNode(index);

    graph_->nodes().erase(index);
    nodes_.endRemoveRows();

    graphChanged();
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
    int row = endPoints_.size();
    beginInsertRows(QModelIndex(), row, row);
    endPoints_.push_back({sourceDropArea, targetDropArea});
    endInsertRows();
}

void ParameterEndPointsModel::remove(const Node *node)
{
    endPoints_.erase(std::remove_if(endPoints_.begin(), endPoints_.end(),
                                    [&node](std::pair<QQuickItem *, QQuickItem *> &endPoints) -> bool
                                    {
                                        auto &[sourceDropArea, targetDropArea] = endPoints;
                                        auto sourceParentNode = sourceDropArea->property("parentNodeBox").value<QObject *>();
                                        auto sourceParentNodeName =
                                            sourceParentNode->property("nodeName").toString().toStdString();
                                        auto targetParentNode = targetDropArea->property("parentNodeBox").value<QObject *>();
                                        auto targetParentNodeName =
                                            targetParentNode->property("nodeName").toString().toStdString();
                                        return sourceParentNodeName == node->name() || targetParentNodeName == node->name();
                                    }),
                     endPoints_.end());
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