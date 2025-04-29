// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "gui/models/nodeGraph/graphModelBase.h"
#include "gui/models/nodeGraph/graphNodeModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"
#include "nodes/graph.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>

class GraphNodeModel;

// This is the base class for any node graph type
class GraphModel : public GraphModelBase
{
    friend GraphNodeModel;

    public:
    GraphModel(Graph &graph) : graph_(graph), nodes_(this) {}

    public:
    // Access the acutal nodes in the model
    Graph &graph() { return graph_; }

    // Access the GraphNodeModel
    QAbstractListModel *nodes() override { return &nodes_; }

    // The number of nodes
    int count() override { return nodes_.rowCount(); }

    // Add a new node at a specific position
    void emplace_back(int x, int y, QVariant type, QVariant name)
    {
        nodes_.beginInsertRows({}, graph_.nodes().size(), graph_.nodes().size() + 1);
        auto node = graph_.addNode(type.toString().toStdString(), type.toString().toStdString());
        NodeWrapper item(*node);
        item.posx = x;
        item.posy = y;
        item.rawValue().setName("Unnamed");
        nodes_.endInsertRows();
        graphChanged();
    }

    // Remove a node
    void deleteNode(std::string index) override
    {
        // List of edges to remove
        auto deadEdges = edges_.deleteNode(index);

        for (auto &edge : deadEdges)
        {
            if (edge.source == index)
                Q_EMIT(nodes_.dataChanged(nodes_.index(indexByName(edge.destination)),
                                          nodes_.index(indexByName(edge.destination))));
        }

        graphChanged();
    }

    protected:
    // The abstract data model for the nodes
    GraphNodeModel nodes_;
    // The graph being modeled
    Graph &graph_;
    // Graph nodes wrapped in the wrappers
    std::vector<NodeWrapper> wrapped_;
};
