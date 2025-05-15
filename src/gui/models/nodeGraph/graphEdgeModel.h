// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <algorithm>
#include <qnamespace.h>

/** A structure that represents the connection between two nodes **/
struct GraphRawEdge
{
    // The node index of the source node
    std::string source;
    // The index of the specific data *within* the source node
    int sourceIndex;
    // The node index of the destination node
    std::string destination;
    // The index of the specific data *within* the destination node
    int destinationIndex;
    // Equality comparison (we get these for free in C++20
    bool operator==(const GraphRawEdge &other) const;
};

/** A model to keep track of the edges between the nodes in the graph.
 * Note that the model only maintains a record of the *existing* edges
 * between nodes in the model.  The GraphModel is the source of truth
 * and this class just caches all of the edges.**/
class GraphEdgeModel : public QAbstractListModel
{
    public:
    GraphEdgeModel();
    GraphEdgeModel(const GraphEdgeModel &other);

    // Remove an edge from the model (by index). Returns false if edge does not exist
    bool dropEdge(std::size_t edge);

    // Remove an edge by value
    bool dropEdge(GraphRawEdge &edge);

    // Create a new edge
    void addEdge(std::string source, int sourceIndex, std::string destination, int destinationIndex);

    // Get all edges connected to a node
    std::vector<GraphRawEdge> connectedTo(std::string index)
    {
        std::vector<GraphRawEdge> result;
        std::copy_if(edgeCache_.begin(), edgeCache_.end(), std::back_inserter(result),
                     [index](auto edge) { return index == edge.source || index == edge.destination; });
        return result;
    }

    std::vector<GraphRawEdge> deleteNode(std::string index)
    {
        auto result = connectedTo(index);
        for (auto &edge : result)
            dropEdge(edge);
        return result;
    }

    // Create a new edge
    void addEdge(GraphRawEdge newEdge);

    GraphEdgeModel &operator=(const GraphEdgeModel &other);

    bool operator!=(const GraphEdgeModel &other);

    // Return number of edges (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;

    private:
    std::vector<GraphRawEdge> edgeCache_;
};
