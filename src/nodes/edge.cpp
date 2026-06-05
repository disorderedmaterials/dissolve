// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/edge.h"
#include "nodes/graph.h"
#include "nodes/loopBack.h"
#include "nodes/outputs.h"

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
    auto sourceNode = parent->findNode(definition.sourceNode);
    if (!sourceNode)
    {
        Messenger::error("Source node '{}' does not exist in the graph.\n", definition.sourceNode);
        return {};
    }
    auto sourceOutput = sourceNode->findOutput(definition.sourceOutput);
    if (!sourceOutput)
    {
        Messenger::error("Source node '{}' has no output parameter '{}'.\n", definition.sourceNode, definition.sourceOutput);
        return {};
    }

    // Confirm that the source is actually an output
    if (!sourceOutput->flags().isSet(ParameterBase::ParameterFlags::Output))
    {
        Messenger::error("Source node '{}' has parameter '{}' but it is not an output.\n", definition.sourceNode,
                         definition.sourceOutput);
        return {};
    }

    // Get target node and input
    auto targetNode = parent->findNode(definition.targetNode);
    if (!targetNode)
    {
        Messenger::error("Target node '{}' does not exist in the graph.\n", definition.targetNode);
        return {};
    }

    // Disallow circular edges (mostly a check for Graph -> Graph connections)
    if (targetNode == parent)
    {
        Messenger::error("Target node is graph '{}' and cannot be the owner of the edge.", definition.targetNode);
        return {};
    }

    // We need to check carefully the target node, since we need to permit outside connections to the Graph object itself as
    // well as its Outputs node explicitly.
    std::shared_ptr<ParameterBase> targetInput{nullptr};
    if (dynamic_cast<Graph *>(targetNode))
    {
        // The target node is a Graph: create a parameter link from the sourceOutput and from it a mapped input
        auto graphNode = dynamic_cast<Graph *>(targetNode);
        auto link = sourceOutput->createParameterLink(definition.targetInput);
        if (!graphNode->addProxyInput(link.inputParameter, link.outputParameter))
        {
            Messenger::error("Failed to add mapped input '{}'.\n", definition.targetInput);
            return {};
        }
        targetInput = link.inputParameter;
    }
    else if (dynamic_cast<OutputsNode *>(targetNode))
    {
        // The target node is the parent Graph's own Outputs node, so create a parameter link from the sourceOutput and from it
        // a mapped output
        auto link = sourceOutput->createParameterLink(definition.targetInput);
        if (!parent->addProxyOutput(link.inputParameter, link.outputParameter))
        {
            Messenger::error("Failed to add mapped output '{}'.\n", definition.targetInput);
            return {};
        }
        targetInput = link.inputParameter;
    }
    else
        targetInput = targetNode->findInput(definition.targetInput);

    if (!targetInput)
    {
        Messenger::error("Target node '{}' has no input parameter '{}'.\n", definition.targetNode, definition.targetInput);
        return {};
    }

    // Confirm that the destination input is actually an input
    if (!targetInput->flags().isSet(ParameterBase::ParameterFlags::Input) && !dynamic_cast<LoopBacksNode *>(targetNode))
    {
        Messenger::error("Target node '{}' has parameter '{}' but it is not an input.\n", definition.targetNode,
                         definition.targetInput);
        return {};
    }

    // Check that types are compatible
    if (!targetInput->acceptsDataFromSource(sourceOutput.get()))
    {
        Messenger::error("Source output ({}@{}) and target input ({}@{}) edge types are not compatible - {} vs {}.\n",
                         definition.sourceOutput, sourceNode->name(), definition.targetInput, targetNode->name(),
                         sourceOutput->storedDataType().name(), targetInput->storedDataType().name());
        return {};
    }

    // Create the edge
    auto edge = std::make_unique<EdgeConstructor>(*sourceNode, *sourceOutput, *targetNode, *targetInput);

    // Notify nodes about the new edge
    if (!sourceNode->linkEdge(edge.get()) || !targetNode->linkEdge(edge.get()))
    {
        Messenger::error("Failed to link edge between source output {}@{} and target input {}@{}.\n", definition.sourceOutput,
                         sourceNode->name(), definition.targetInput, targetNode->name());
        return {};
    }

    return edge;
}

// Return source node
Node &Edge::sourceNode() const { return sourceNode_; }

// Return source output parameter
const ParameterBase &Edge::sourceOutput() const { return sourceOutput_; }

// Return target node
Node &Edge::targetNode() const { return targetNode_; }

// Return target input parameter
ParameterBase &Edge::targetInput() const { return targetInput_; }

// Return definition for the edge
EdgeDefinition Edge::definition() const
{
    return {std::string(sourceNode_.name()), std::string(sourceOutput_.name()), std::string(targetNode_.name()),
            std::string(targetInput_.name())};
}

EdgeDefinition::EdgeDefinition(std::string srcNode, std::string srcOutput, std::string tgtNode, std::string tgtInput)
    : sourceNode(srcNode), sourceOutput(srcOutput), targetNode(tgtNode), targetInput(tgtInput)
{
}

EdgeDefinition::EdgeDefinition(const EdgeDefinition &other)
{
    sourceNode = other.sourceNode;
    sourceOutput = other.sourceOutput;
    targetNode = other.targetNode;
    targetInput = other.targetInput;
}

bool EdgeDefinition::operator==(const EdgeDefinition &other) const
{
    return sourceNode == other.sourceNode && sourceOutput == other.sourceOutput && targetNode == other.targetNode &&
           targetInput == other.targetInput;
}

// Return as a string
std::string EdgeDefinition::asString() const
{
    return std::format("Edge( [O] {}@{} -> {}@{} [I] )", sourceOutput, sourceNode, targetInput, targetNode);
}

/*
 * Serialisation
 */

// Express as a serialisable value
void EdgeDefinition::serialise(std::string tag, SerialisedValue &target) const
{
    SerialisedValue result;
    result["sourceNode"] = sourceNode;
    result["sourceOutput"] = sourceOutput;
    result["targetNode"] = targetNode;
    result["targetInput"] = targetInput;
    target[tag] = result;
}

// Read values from a serialisable value
void EdgeDefinition::deserialise(const SerialisedValue &node)
{
    sourceNode = toml::find<std::string>(node, "sourceNode");
    sourceOutput = toml::find<std::string>(node, "sourceOutput");
    targetNode = toml::find<std::string>(node, "targetNode");
    targetInput = toml::find<std::string>(node, "targetInput");
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

        // Copy the parameter data over
        if (!targetInput_.assignDataFromSource(&sourceOutput_))
        {
            Messenger::error("Failed to assign value from {}@{} to {}@{}.\n", sourceOutput_.name(), sourceNode_.name(),
                             targetInput_.name(), targetNode_.name());
            return NodeConstants::ProcessResult::Failed;
        }

        // All succeeded, so update version index
        sourceNodeVersionIndex_ = sourceNode_.versionIndex();

        return NodeConstants::ProcessResult::Success;
    }

    return NodeConstants::ProcessResult::Unchanged;
}

// Pull the data from the source node to the target, returning a ProcessResult
NodeConstants::ProcessResult LoopEdge::pull()
{
    // Copy the parameter data over
    if (!analogue().assignDataFromSource(&sourceOutput_))
        return NodeConstants::ProcessResult::Failed;

    sourceNode().setUpdateRequired();

    // All succeeded, so update version index
    sourceNodeVersionIndex_ = sourceNode_.versionIndex();

    return NodeConstants::ProcessResult::Success;
}

// The constructor is private because it can only be constructed by the factory method
LoopEdge::LoopEdge(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput)
    : Edge(sourceNode, sourceOutput, targetNode, targetInput)
{
    auto analogousEdge = targetNode.outputEdges().find(targetInput.name())->second[0];
    analogue_ = &analogousEdge->sourceOutput_;
}

// Make a loop edge from a given node output (LoopBack), which feeds back into the graph's inputs node
std::unique_ptr<LoopEdge> LoopEdge::makeLoopEdge(const Edge *edge, Node &inputs)
{
    return std::unique_ptr<LoopEdge>(new LoopEdge(edge->sourceNode_, edge->sourceOutput_, inputs, edge->targetInput_));
}

// Return the analogue parameter, which the loop edge source corresponds to
ParameterBase &LoopEdge::analogue() { return *analogue_; }

// Ensure next call to pull() will retrieve the data from the source node
void Edge::forceNextPull() { sourceNodeVersionIndex_ = NodeConstants::InvalidVersion; }

/*
 * I/O
 */

// Express as a serialisable value
void Edge::serialise(std::string tag, SerialisedValue &target) const { return definition().serialise(tag, target); }

// Read values from a serialisable value. This is required for the
// SerialisableValue type implementation, but we actually deserialise
// Edges through an EdgeConnection.  I've added this error to
// immediately alert us in case this function is ever called.
void Edge::deserialise(const SerialisedValue &node)
{
    throw std::runtime_error("Cannot directly deserialise edges.  Please contact the Dissolve development team if you are "
                             "seeing this error - this is a bug and NOT your fault.\n");
}
