// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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
    auto node = graph_->createNode(type.toString().toStdString(), type.toString().toStdString());
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

void GraphModel::handleReset() { Q_EMIT(graphChanged()); }
