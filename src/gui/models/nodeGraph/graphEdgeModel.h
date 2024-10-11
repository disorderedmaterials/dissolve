// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <qnamespace.h>
#include <variant>

/** A list model that provides the tick labels of the axis */
class GraphEdgeModel : public QAbstractListModel
{
    public:
    GraphEdgeModel() {}
    GraphEdgeModel(const GraphEdgeModel &other) : edgeCache_(other.edgeCache_) {}

    std::vector<std::array<int, 4>> &edgeCache() { return edgeCache_; }

    void dropEdge(int edge)
    {
        beginRemoveRows({}, edge, edge);
        edgeCache_.erase(edgeCache_.begin() + edge);
        endRemoveRows();
    }

    void addEdge(int source, int sourceIndex, int destination, int destinationIndex)
    {
        beginInsertRows({}, edgeCache_.size(), edgeCache_.size());
        auto &newEdge = edgeCache_.emplace_back();
        newEdge[0] = source;
        newEdge[1] = sourceIndex;
        newEdge[2] = destination;
        newEdge[3] = destinationIndex;
        endInsertRows();
    }

    GraphEdgeModel &operator=(const GraphEdgeModel &other)
    {
        edgeCache_ = other.edgeCache_;
        return *this;
    }
    bool operator!=(const GraphEdgeModel &other) { return edgeCache_ != other.edgeCache_; }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return edgeCache_.size(); }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        auto row = index.row();
        if (row >= edgeCache_.size())
            return {};
        auto &edge = edgeCache_[row];

        switch (role - Qt::UserRole)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                return edge[role - Qt::UserRole];
            default:
                return {};
        }
    }
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole] = "source";
        roles[Qt::UserRole + 1] = "sourceIndex";
        roles[Qt::UserRole + 2] = "destination";
        roles[Qt::UserRole + 3] = "destIndex";
        return roles;
    }

    private:
    std::vector<std::array<int, 4>> edgeCache_;
};
