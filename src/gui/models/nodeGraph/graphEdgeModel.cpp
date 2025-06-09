// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphEdgeModel.h"
#include "gui/models/nodeGraph/graphModel.h"
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
    if (edge >= edges_().size())
        return false;
    beginRemoveRows({}, edge, edge);
    edges_().erase(edges_().begin() + edge);
    endRemoveRows();
    return true;
}

// Remove an edge by value.  Returns false if the edge does not exist
bool GraphEdgeModel::dropEdge(Edge &edge)
{
    auto index = std::find_if(edges_().begin(), edges_().end(), [&edge](auto &item) { return &edge == item.get(); });
    // Check if edge is found
    if (index == edges_().end())
        return false;
    else
        return dropEdge(index - edges_().begin());
}

// Create a new edge
void GraphEdgeModel::addEdge(Edge &newEdge)
{
    beginInsertRows({}, edges_().size(), edges_().size());
    edges_().emplace_back(std::make_unique<Edge>(newEdge));
    endInsertRows();
}

int GraphEdgeModel::rowCount(const QModelIndex &parent) const
{
    if (!graph_)
        return 0;
    return edges_().size();
}

QVariant GraphEdgeModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= edges_().size())
        return {};
    auto &edge = edges_()[row];

    auto source = std::find_if(parent_->wrapped_.begin(), parent_->wrapped_.end(),
                               [&edge](const auto &x) { return &x.rawValue() == &edge->sourceNode(); });

    auto target = std::find_if(parent_->wrapped_.begin(), parent_->wrapped_.end(),
                               [&edge](const auto &x) { return &x.rawValue() == &edge->targetNode(); });

    switch (role)
    {
        case Role::SOURCE_X:
            return source != parent_->wrapped_.end() ? source->posx : 0;
        case Role::SOURCE_Y:
            return source != parent_->wrapped_.end() ? source->posy : 0;
        case Role::TARGET_X:
            return target != parent_->wrapped_.end() ? target->posx : 100;
        case Role::TARGET_Y:
            return target != parent_->wrapped_.end() ? target->posy : 0;
        default:
            return {};
    }
}

QHash<int, QByteArray> GraphEdgeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Role::SOURCE_X] = "sourceX";
    roles[Role::SOURCE_Y] = "sourceY";
    roles[Role::TARGET_X] = "targetX";
    roles[Role::TARGET_Y] = "targetY";
    return roles;
}

void GraphEdgeModel::updatePosition(const int idx)
{
    const auto &node = parent_->wrapped_[idx].rawValue();
    for (auto j = 0; j < graph_->edges().size(); ++j)
    {
        const auto &edge = graph_->edges()[j];
        if (&edge->sourceNode() == &node)
            Q_EMIT dataChanged(index(j), index(j + 1), {Role::SOURCE_X, Role::SOURCE_Y});
        if (&edge->targetNode() == &node)
            Q_EMIT dataChanged(index(j), index(j + 1), {Role::TARGET_X, Role::TARGET_Y});
    }
}

// The edges of the graph
Graph::Edges &GraphEdgeModel::edges_() { return graph_->edges(); }
const Graph::Edges &GraphEdgeModel::edges_() const { return graph_->edges(); }
