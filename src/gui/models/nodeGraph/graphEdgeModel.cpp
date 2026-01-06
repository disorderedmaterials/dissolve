// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphEdgeModel.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "nodes/edge.h"
#include <qnamespace.h>

enum Role
{
    SOURCE_X = Qt::UserRole,
    SOURCE_Y,
    TARGET_X,
    TARGET_Y,
};

GraphEdgeModel::GraphEdgeModel(GraphModel *parent, Graph *&graph) : parent_(parent), graph_(graph) {}

GraphEdgeModel::GraphEdgeModel(const GraphEdgeModel &other) : graph_(other.graph_) {}

// Remove an edge from the model (by index). Returns false if edge does not exist
bool GraphEdgeModel::dropEdge(std::size_t edge)
{
    // Check if edge is in range
    if (edge >= edges().size())
        return false;
    beginRemoveRows({}, edge, edge);
    edges().erase(edges().begin() + edge);
    endRemoveRows();
    return true;
}

// Remove an edge by value.  Returns false if the edge does not exist
bool GraphEdgeModel::dropEdge(Edge &edge)
{
    auto index = std::find_if(edges().begin(), edges().end(), [&edge](auto &item) { return &edge == item.get(); });
    // Check if edge is found
    if (index == edges().end())
        return false;
    else
        return dropEdge(index - edges().begin());
}

// Create a new edge
void GraphEdgeModel::addEdge(Edge &newEdge)
{
    beginInsertRows({}, edges().size(), edges().size());
    edges().emplace_back(std::make_unique<Edge>(newEdge));
    endInsertRows();
}

void GraphEdgeModel::addEdge(EdgeDefinition &newEdge)
{
    auto edge = Edge::create(parent_->graph(), newEdge);
    if (edge)
        addEdge(*edge);
}

// Return number of edges (required by QAbstractListModel)
int GraphEdgeModel::rowCount(const QModelIndex &parent) const
{
    if (!graph_)
        return 0;
    for (auto &edge : edges())
        std::cout << edge->definition().asString() << std::endl;
    return edges().size();
}

QVariant GraphEdgeModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= edges().size())
        return {};
    auto &edge = edges()[row];

    auto source = std::find_if(parent_->wrapped_.begin(), parent_->wrapped_.end(),
                               [&edge](const auto &x) { return &x.rawValue() == &edge->sourceNode(); });

    auto target = std::find_if(parent_->wrapped_.begin(), parent_->wrapped_.end(),
                               [&edge](const auto &x) { return &x.rawValue() == &edge->targetNode(); });

    std::optional<QPointF> sourceOffset, targetOffset;

    if (source != parent_->wrapped_.end())
    {
        auto it = source->outputPos.find(std::string(edge->sourceOutput().name()));
        if (it != source->outputPos.end())
            sourceOffset = it->second;
    }

    if (target != parent_->wrapped_.end())
    {
        auto it = target->inputsPos.find(std::string(edge->targetInput().name()));
        if (it != target->inputsPos.end())
            targetOffset = it->second;
    }

    switch (role)
    {
        case Role::SOURCE_X:
            return source->rawValue().x + (sourceOffset ? sourceOffset->x() : 0);
        case Role::SOURCE_Y:
            return source->rawValue().y + (sourceOffset ? sourceOffset->y() : 0);
        case Role::TARGET_X:
            return target->rawValue().x + (targetOffset ? targetOffset->x() : 0);
        case Role::TARGET_Y:
            return target->rawValue().y + (targetOffset ? targetOffset->y() : 0);
        default:
            return {};
    }
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> GraphEdgeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Role::SOURCE_X] = "sourceX";
    roles[Role::SOURCE_Y] = "sourceY";
    roles[Role::TARGET_X] = "targetX";
    roles[Role::TARGET_Y] = "targetY";
    return roles;
}

// Update all edges connected to the node at idx
void GraphEdgeModel::updatePosition(const int idx)
{
    const auto &node = parent_->wrapped_[idx].rawValue();
    for (auto j = 0; j < graph_->edges().size(); ++j)
    {
        const auto &edge = graph_->edges()[j];
        if (&edge->sourceNode() == &node)
            Q_EMIT dataChanged(index(j), index(j), {Role::SOURCE_X, Role::SOURCE_Y});
        else if (&edge->targetNode() == &node)
            Q_EMIT dataChanged(index(j), index(j), {Role::TARGET_X, Role::TARGET_Y});
    }
}

// The edges of the graph
Graph::Edges &GraphEdgeModel::edges() { return graph_->edges(); }
const Graph::Edges &GraphEdgeModel::edges() const { return graph_->edges(); }

// Drop cache and pull all data from graph
void GraphEdgeModel::reset()
{
    beginResetModel();
    endResetModel();
}
