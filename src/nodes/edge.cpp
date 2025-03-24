// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/edge.h"
#include "nodes/graph.h"

Edge::Edge(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput)
    : sourceNode_(sourceNode), sourceOutput_(sourceOutput), targetNode_(targetNode), targetInput_(targetInput)
{
}

Edge::~Edge()
{
    // Detach from the source and target nodes
    sourceNode_.unlinkEdge(this);
    targetNode_.unlinkEdge(this);
}

// Local EdgeConstructor class to allow creation of memory-managed Edge instances
class EdgeConstructor : public Edge
{
    public:
    EdgeConstructor(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput)
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
    auto edge = std::make_unique<EdgeConstructor>(*sourceNode, *sourceOutput, *targetNode, *targetInput);

    // Notify nodes about the new edge
    if (!sourceNode->linkEdge(edge.get()) || !targetNode->linkEdge(edge.get()))
        return {};

    return edge;
}

// Return source node
Node &Edge::sourceNode() const { return sourceNode_; }

// Return source output parameter
const ParameterBase &Edge::sourceOutput() const { return sourceOutput_; }

// Return target node
Node &Edge::targetNode() const { return targetNode_; }

// Return target input parameter
const ParameterBase &Edge::targetInput() const { return targetInput_; }

// Return definition for the edge
EdgeDefinition Edge::definition() const
{
    return {std::string(sourceNode_.name()), std::string(sourceOutput_.name()), std::string(targetNode_.name()),
            std::string(targetInput_.name())};
}

// Pull the data from the source node to the target, returning a ProcessResult
NodeConstants::ProcessResult Edge::pull()
{
    /*
     * If the versionIndex for the source node stored in the Edge is Invalid or different to that on the source node
     * itself we need to pull from or run the source node to get the updated output. We can then store the source node's
     * current versionIndex ready for next time.
     */
    if (!sourceNode_.isUpToDate() || (sourceNodeVersionIndex_ != sourceNode_.versionIndex()))
    {
        auto result = sourceNode_.run();
        if (result != NodeConstants::ProcessResult::Success && result != NodeConstants::ProcessResult::Unchanged)
        {
            Messenger::error("Failed to pull updated value from node '{}'\n", sourceNode_.name());
            return result;
        }

        // Update version index
        sourceNodeVersionIndex_ = sourceNode_.versionIndex();

        // Copy the parameter data over
        return targetInput_.assign(&sourceOutput_) ? NodeConstants::ProcessResult::Success
                                                   : NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Unchanged;
}
