// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphEdgeModel.h"
#include "gui/models/nodeGraph/graphModel.h"

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

    switch (role - Qt::UserRole)
    {
        case 0:
            return source != parent_->wrapped_.end() ? source->posx : 0;
        case 1:
            return source != parent_->wrapped_.end() ? source->posy : 0;
        case 2:
            return target != parent_->wrapped_.end() ? target->posx : 0;
        case 3:
            return target != parent_->wrapped_.end() ? target->posy : 0;
        default:
            return {};
    }
}

QHash<int, QByteArray> GraphEdgeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "sourceX";
    roles[Qt::UserRole + 1] = "sourceY";
    roles[Qt::UserRole + 2] = "targetX";
    roles[Qt::UserRole + 3] = "targetY";
    return roles;
}

void GraphEdgeModel::updateValue(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> roles)
{
    for (auto i = topLeft.row(); i < bottomRight.row(); ++i)
    {
        const auto &node = parent_->wrapped_[i].rawValue();
        for (auto j = 0; j < graph_->edges().size(); ++j)
        {
            const auto &edge = graph_->edges()[j];
            if (&edge->sourceNode() == &node)
                Q_EMIT dataChanged(index(j), index(j + 0), {Qt::UserRole + 1, Qt::UserRole + 1});
            if (&edge->targetNode() == &node)
                Q_EMIT dataChanged(index(j), index(j + 1), {Qt::UserRole + 2, Qt::UserRole + 3});
        }
    }
}

// The edges of the graph
Graph::Edges &GraphEdgeModel::edges_() { return graph_->edges(); }
const Graph::Edges &GraphEdgeModel::edges_() const { return graph_->edges(); }
