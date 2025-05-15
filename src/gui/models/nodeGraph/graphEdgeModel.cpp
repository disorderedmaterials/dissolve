// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/nodeGraph/graphEdgeModel.h"

bool GraphRawEdge::operator==(const GraphRawEdge &other) const
{
    return source == other.source && sourceIndex == other.sourceIndex && destination == other.destination &&
           destinationIndex == other.destinationIndex;
}

GraphEdgeModel::GraphEdgeModel() {}

GraphEdgeModel::GraphEdgeModel(const GraphEdgeModel &other) : edgeCache_(other.edgeCache_) {}

// Remove an edge from the model (by index). Returns false if edge does not exist
bool GraphEdgeModel::dropEdge(std::size_t edge)
{
    // Check if edge is in range
    if (edge >= edgeCache_.size())
        return false;
    beginRemoveRows({}, edge, edge);
    edgeCache_.erase(edgeCache_.begin() + edge);
    endRemoveRows();
    return true;
}

// Remove an edge by value.  Returns false if the edge does not exist
bool GraphEdgeModel::dropEdge(GraphRawEdge &edge)
{
    auto index = std::find(edgeCache_.begin(), edgeCache_.end(), edge);
    // Check if edge is found
    if (index == edgeCache_.end())
        return false;
    else
        return dropEdge(index - edgeCache_.begin());
}

// Create a new edge
void GraphEdgeModel::addEdge(std::string source, int sourceIndex, std::string destination, int destinationIndex)
{
    GraphRawEdge edge{source, sourceIndex, destination, destinationIndex};
    addEdge(edge);
}

// Create a new edge
void GraphEdgeModel::addEdge(GraphRawEdge newEdge)
{
    beginInsertRows({}, edgeCache_.size(), edgeCache_.size());
    edgeCache_.emplace_back(newEdge);
    endInsertRows();
}

GraphEdgeModel &GraphEdgeModel::operator=(const GraphEdgeModel &other)
{
    edgeCache_ = other.edgeCache_;
    return *this;
}

bool GraphEdgeModel::operator!=(const GraphEdgeModel &other) { return edgeCache_ != other.edgeCache_; }

int GraphEdgeModel::rowCount(const QModelIndex &parent) const { return edgeCache_.size(); }

QVariant GraphEdgeModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= edgeCache_.size())
        return {};
    auto &edge = edgeCache_[row];

    switch (role - Qt::UserRole)
    {
        case 0:
            return QString(edge.source.c_str());
        case 1:
            return edge.sourceIndex;
        case 2:
            return QString(edge.destination.c_str());
        case 3:
            return edge.destinationIndex;
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
