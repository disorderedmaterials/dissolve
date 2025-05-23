// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/node.h"
#include "base/sysFunc.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include <algorithm>

/*
 * Definition
 */

// Set node name
void Node::setName(std::string_view newName)
{
    if (parentGraph_)
        parentGraph_->setNodeName(this, newName);
    else
        error("Can't set node name to '{}' as it is not part of a Graph.", newName);
}

// Return node name
std::string_view Node::name() const { return parentGraph_ ? parentGraph_->nodeName(this) : "UnparentedNode"; }

/*
 * Node message
 */

// Print latest message
bool Node::echo_ = false;

// Message store vector
const Node::MessageStore &Node::messages() const { return messages_; }

// Returns true if message with given status exists
bool Node::hasMessages(MessageStatus status) const
{
    return std::any_of(messages_.cbegin(), messages_.cend(), [status](const auto &msg) { return msg.first == status; });
}

// Print latest message
void Node::echo() { std::cout << messages_.back().second << std::endl; }

/*
 * Processing & Validity
 */

// Return version index for the node, bumped whenever result outputs change
int Node::versionIndex() const { return versionIndex_; }

// Invalidate the current node, resetting the version index
void Node::invalidate()
{
    versionIndex_ = NodeConstants::InvalidVersion;
    clearData();
}

// Flag that the node data needs to be updated
void Node::setUpdateRequired()
{
    // If already flagged then do nothing
    if (!isUpToDate())
        return;

    upToDate_ = false;

    // Make sure all output edges propagate this information down
    for (auto &&[outputName, edge] : outputEdges())
        if (!edge->targetInput().flags().isSet(ParameterBase::ParameterFlags::NoUpdate))
            edge->targetInput().setParentUpdateRequired();
}

// Return whether the node's data is up-to-date
bool Node::isUpToDate() const { return upToDate_; }

// Check that all required inputs are present, and that all inputs are valid
bool Node::inputsAreValid() const
{
    for (auto &[inputName, parameter] : inputs_)
    {
        // Does this input have a link or links?
        if (inputEdges_.contains(inputName))
        {
            if (!inputEdges_.at(inputName)->sourceOutput().parent()->inputsAreValid())
                return false;
        }
        else if (parameter->flags().isSet(ParameterBase::ParameterFlags::Required))
            return false;
    }

    return true;
}

// Run the node, retrieving dependent inputs as necessary
NodeConstants::ProcessResult Node::run()
{
    // Check our input links - if any are out-of-date we must retrieve new values. This will automatically unset upToDate_
    for (auto &[inputName, edge] : inputEdges_)
    {
        auto edgeResult = edge->pull();
        switch (edgeResult)
        {
            case (NodeConstants::ProcessResult::Failed):
            case (NodeConstants::ProcessResult::InputsNotSatisfied):
                return NodeConstants::ProcessResult::Failed;
            case (NodeConstants::ProcessResult::Success):
            case (NodeConstants::ProcessResult::Unchanged):
                break;
        }
    }

    // If input links have updated or we are currently flagged as invalid we must reprocess
    auto result = NodeConstants::ProcessResult::Unchanged;
    if (!upToDate_ || versionIndex_ == NodeConstants::InvalidVersion)
    {
        result = process();
        switch (result)
        {
            case (NodeConstants::ProcessResult::Failed):
            case (NodeConstants::ProcessResult::InputsNotSatisfied):
                break;
            case (NodeConstants::ProcessResult::Success):
                ++versionIndex_;
                upToDate_ = true;
                break;
            case (NodeConstants::ProcessResult::Unchanged):
                upToDate_ = true;
                break;
        }
    }

    return result;
}

// Perform processing
NodeConstants::ProcessResult Node::process() { return NodeConstants::ProcessResult::Failed; }

/*
 * Inputs, Outputs & Options
 */

// Link edge, returning whether we accept it
bool Node::linkEdge(Edge *edge)
{

    // The supplied Edge was created via our parent Graph, but we will still check to see whether we accept it
    if (&edge->targetNode() == this)
    {
        // We are the target node, so we will double-check the specified input to see if it can accept the connection
        // Simple check at present, we accept at most one connection per input, so if one already exists we complain
        if (inputEdges_.contains(edge->targetInput().name()))
            return Messenger::error("Node '{}' refusing to accept Edge connecting to input '{}' as one already exists.\n",
                                    name(), edge->targetInput().name());

        // All good, so add the input to our list
        inputEdges_[edge->targetInput().name()] = edge;

        // Adding an Edge to an input always invalidates the target
        invalidate();
    }
    else if (&edge->sourceNode() == this)
    {
        // We are the source node - add the outgoing edge to our list
        outputEdges_[edge->sourceOutput().name()] = edge;
    }
    else
        return Messenger::error("Node '{}' is neither the source nor the target for the supplied Edge.\n", name());

    return true;
}

// Unlink edge
void Node::unlinkEdge(Edge *edge)
{
    // If we are the Edge's targetNode_ then we should have its pointer in inputEdges_
    if (&edge->targetNode() == this)
    {
        auto it = std::find_if(inputEdges_.begin(), inputEdges_.end(),
                               [edge](const auto &inputEdge) { return edge == inputEdge.second; });
        if (it == inputEdges_.end())
            Messenger::error("Tried to unlink an incoming edge to target node '{}' which knew nothing about it.\n", name());
        else
        {
            inputEdges_.erase(it);
            invalidate();
        }
    }
    else if (&edge->sourceNode() == this)
    {
        // We are the source node for the edge...
        auto it = std::find_if(outputEdges_.begin(), outputEdges_.end(),
                               [edge](const auto &outputEdge) { return edge == outputEdge.second; });
        if (it == outputEdges_.end())
            Messenger::error("Tried to unlink an outgoing edge from source node '{}' which knew nothing about it.\n", name());
        else
            outputEdges_.erase(it);
    }
    else
        Messenger::error("Node '{}' is neither the source nor the target for the Edge being unlinked.\n", name());
}

// Own supplied parameter
bool Node::ownParameter(std::shared_ptr<ParameterBase> &parameter, bool isOutput)
{
    if ((!isOutput && findInput(parameter->name())) || (isOutput && findOutput(parameter->name())))
        return Messenger::error("Parameter '{}' cannot be owned as it conflicts with an existing input or output.\n",
                                parameter->name());

    parameter->setParent(this);

    if (isOutput)
        outputs_.emplace(parameter->name(), parameter);
    else
        inputs_.emplace(parameter->name(), parameter);

    return true;
}

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findInput(std::string_view name) const
{
    if (!inputs_.contains(std::string{name}))
        return {};
    return inputs_.at(std::string{name});
}

// Return input parameters
Node::NodeParameterMap &Node::inputs() { return inputs_; };

// Return named output parameter if it exists
std::shared_ptr<ParameterBase> Node::findOutput(std::string_view outputName) const
{
    if (!outputs_.contains(std::string{outputName}))
        return {};
    return outputs_.at(std::string{outputName});
}

// Return output parameters
Node::NodeParameterMap &Node::outputs() { return outputs_; };

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findOption(std::string_view optionName) const
{
    if (!options_.contains(std::string{optionName}))
        return {};
    return options_.at(std::string{optionName});
}

// Return Options
Node::NodeParameterMap &Node::options() { return options_; };

// Get the incoming edges to this node
Node::EdgeMap &Node::inputEdges() { return inputEdges_; }

// Get the outgoing edges from this node
Node::EdgeMap &Node::outputEdges() { return outputEdges_; }

// Returns the node parent graph
Graph *Node::parentGraph() const { return parentGraph_; }

// Return the Dissolve reference
Dissolve &Node::dissolve() const { return parentGraph_->dissolve(); }

// Return the world pool
const ProcessPool &Node::processPool() const { return parentGraph_->processPool(); }

/*
 * Data
 */

// Clear any local data
void Node::clearData() {}

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue Node::serialise() const
{
    SerialisedValue result, inputs, outputs, options;
    result["name"] = name();
    result["type"] = type();

    fromMap(inputs_, "inputs", result, [](const auto k, const auto v) { return !v->isDefault(); });
    fromMap(options_, "options", result, [](const auto k, const auto v) { return !v->isDefault(); });
    return result;
}

// Read values from a serialisable value
void Node::deserialise(const SerialisedValue &node)
{
    toMap(node, "inputs",
          [this](const auto &k, const auto &v)
          {
              if (inputs_.contains(k))
                  inputs_[k]->deserialise(v);
              else
                  Messenger::exception("Node {} does not contain a parameter {}", name(), k);
          });
    toMap(node, "options",
          [this](const auto &k, const auto &v)
          {
              if (options_.contains(k))
                  options_[k]->deserialise(v);
              else
                  Messenger::exception("Node {} does not contain an option {}", name(), k);
          });
}
