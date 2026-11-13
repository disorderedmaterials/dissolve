// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/graphEdges.h"
#include "gui2/models/graphModel.h"
#include "nodes/edge.h"
#include <qnamespace.h>
#include <stdexcept>

GraphEdges::GraphEdges(GraphModel *parent, Graph *&graph) : parent_(parent), graph_(graph) {}

GraphEdges::GraphEdges(const GraphEdges &other) : graph_(other.graph_) {}

//
Graph *&GraphEdges::graph() { return graph_; }

//
const std::vector<Edge *> &GraphEdges::loop() const { return loop_; }

//
const std::vector<Edge *> &GraphEdges::standard() const { return standard_; }

//
void GraphEdges::update()
{
    standard_.clear();
    loop_.clear();

    // Update loop edges, if any exist
    if (parent_->iterable())
        for (const auto &loopEdge : parent_->iterable()->loopEdges())
            loop_.push_back(loopEdge.get());

    // Update standard edges
    for (const auto &edge : graph_->edges())
        standard_.push_back(edge.get());
}

//
int GraphEdges::count() { return standard_.size() + loop_.size(); }

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
    for (const auto &edge : standard_)
        if (toRemove(edge))
            remove(edge);

    // Remove connected loop edges
    for (const auto &edge : loop_)
        if (toRemove(edge))
            remove(edge);
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
