// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/graphEdges.h"
#include "gui2/models/graphModel.h"
#include "nodes/edge.h"
#include <qnamespace.h>
#include <stdexcept>

GraphEdges::GraphEdges(GraphModel *parent, Graph *&graph)
    : parent_(parent), graph_(graph), standard_(&graph->edges()), loop_(nullptr)
{
}

GraphEdges::GraphEdges(const GraphEdges &other) : graph_(other.graph_) {}

//
Graph *&GraphEdges::graph() { return graph_; }

//
const Graph::Edges *GraphEdges::loop() const { return loop_; }

//
const Graph::Edges *GraphEdges::standard() const { return standard_; }

//
void GraphEdges::setLoopEdges(Graph::Edges *edges) { loop_ = edges; }

//
void GraphEdges::setEdges(Graph::Edges *edges) { standard_ = edges; }

//
int GraphEdges::count() { return (standard_ ? standard_->size() : 0) + (loop_ ? loop_->size() : 0); }

// Remove an edge. Returns false if the edge does not exist
bool GraphEdges::remove(Edge *edge)
{
    if (!graph_->removeEdge(edge->definition()))
        return false;
    Q_EMIT parent_->graphChanged();
    return true;
}

// Remove any edges connected to a node with a given name
void GraphEdges::removeConnected(std::string nodeName)
{
    auto toRemove = [&nodeName](auto &edgePtr)
    {
        auto sourceNodeName = edgePtr->sourceNode().name();
        auto targetNodeName = edgePtr->targetNode().name();
        return (nodeName == sourceNodeName || nodeName == targetNodeName);
    };

    // Remove connected standard edges
    const auto nStandardEdges = standard_->size();
    for (int i = nStandardEdges - 1; i >= 0; --i)
        if (toRemove(standard_->at(i)))
            remove(standard_->at(i).get());

    // Remove connected loop edges, if any exist
    if (loop_)
    {
        const auto nLoopEdges = loop_->size();
        for (int i = nLoopEdges - 1; i >= 0; --i)
            remove(loop_->at(i).get());
    }
}

// Create a new edge. Returns false if the edge could not be created
bool GraphEdges::add(EdgeDefinition &definition)
{
    if (!graph_->addEdge(definition))
        return false;
    Q_EMIT parent_->graphChanged();
    return true;
}

// Toggles the edge edit mode
void GraphEdges::toggleEdgeEditMode()
{
    edgeEditMode_ = !edgeEditMode_;
    Q_EMIT edgeEditModeChanged();
}

// Returns the current edge edit mode
bool GraphEdges::edgeEditMode() { return edgeEditMode_; }
