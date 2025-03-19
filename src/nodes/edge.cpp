// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/edge.h"
#include "nodes/graph.h"

Edge::Edge(Node *sourceNode, ParameterBase &sourceOutput, Node *targetNode, ParameterBase &targetInput)
    : sourceNode_(sourceNode), sourceOutput_(sourceOutput), targetNode_(targetNode), targetInput_(targetInput)
{
}

class EdgeConstructor : public Edge
{
    public:
    EdgeConstructor(Node *sourceNode, ParameterBase &sourceOutput, Node *targetNode, ParameterBase &targetInput)
        : Edge(sourceNode, sourceOutput, targetNode, targetInput)
    {
    }
};

// Create an edge from the supplied definition
std::unique_ptr<Edge> Edge::create(Graph *parent, const EdgeDefinition &definition)
{
    // Get source node and output
    auto sourceNode = parent->node(definition.sourceNode);
    if (!sourceNode)
    {
        Messenger::error("Source node '{}' does not exist in the graph.\n", definition.sourceNode);
        return {};
    }
    auto sourceOutput = sourceNode->findOutput(definition.sourceOutput);
    if (!sourceOutput)
    {
        Messenger::error("Source node '{}' has no parameter '{}'.\n", definition.sourceNode, definition.sourceOutput);
        return {};
    }

    // Confirm that the source is actually an output
    if (!sourceOutput->flags().isSet(ParameterBase::ParameterFlags::Output))
    {
        Messenger::error("Source node '{}' has parameter '{}' but it is not an Output.\n", definition.sourceNode,
                         definition.sourceOutput);
        return {};
    }

    // Get target node and input
    auto targetNode = parent->node(definition.targetNode);
    if (!targetNode)
    {
        Messenger::error("Target node '{}' does not exist in the graph.\n", definition.targetNode);
        return {};
    }
    auto targetInput = targetNode->findInput(definition.targetInput);
    if (!targetInput)
    {
        Messenger::error("Target node '{}' has no parameter '{}'.\n", definition.targetNode, definition.targetInput);
        return {};
    }

    // Confirm that the destination input is actually an input
    if (!targetInput->flags().isSet(ParameterBase::ParameterFlags::Input))
    {
        Messenger::error("Target node '{}' has parameter '{}' but it is not an Input.\n", definition.sourceNode,
                         definition.sourceOutput);
        return {};
    }

    // Check that types are compatible
    if (sourceOutput->type() != targetInput->type())
        return {};

    // Create the edge
    auto edge = std::make_unique<EdgeConstructor>(sourceNode, *sourceOutput, targetNode, *targetInput);

    // Notify nodes about the new edge
    if (!sourceNode->addEdge(edge.get()) || !targetNode->addEdge(edge.get()))
        return {};

    return edge;
}

bool Edge::updateSource()
{
    // TODO
    //    auto update = sourceOutput_.runUpdate();
    //    if (!update)
    //        return false;
    return targetInput_.assign(&sourceOutput_);
}

// Return source node
Node *Edge::sourceNode() const { return sourceNode_; }

// Return source output parameter
const ParameterBase &Edge::sourceOutput() const { return sourceOutput_; }

// Return target node
Node *Edge::targetNode() const { return targetNode_; }

// Return target input parameter
const ParameterBase &Edge::targetInput() const { return targetInput_; }
