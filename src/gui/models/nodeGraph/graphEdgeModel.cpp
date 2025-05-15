// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphEdgeModel.h"

GraphEdgeModel::GraphEdgeModel(Graph *&graph) : graph_(graph) {}

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

    switch (role - Qt::UserRole)
    {
        case 0:
            return QString::fromStdString(std::string(edge->sourceNode().name()));
        case 1:
            return QString::fromStdString(std::string(edge->sourceOutput().name()));
        case 2:
            return QString::fromStdString(std::string(edge->targetNode().name()));
        case 3:
            return QString::fromStdString(std::string(edge->targetInput().name()));
        default:
            return {};
    }
}

QHash<int, QByteArray> GraphEdgeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "source";
    roles[Qt::UserRole + 1] = "sourceIndex";
    roles[Qt::UserRole + 2] = "destination";
    roles[Qt::UserRole + 3] = "destIndex";
    return roles;
}

// The edges of the graph
Graph::Edges &GraphEdgeModel::edges_() { return graph_->edges(); }
const Graph::Edges &GraphEdgeModel::edges_() const { return graph_->edges(); }
