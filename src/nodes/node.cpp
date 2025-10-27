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
    for (auto &&[outputName, edges] : outputEdges())
        for (auto edge : edges)
        {
            auto &input = edge->targetInput();

            if (input.flags().isSet(ParameterBase::ParameterFlags::NoUpdate))
                continue;

            input.setParentUpdateRequired();

            // If the target input is a vector, all edges to it must be marked for re-pull and its data cleared
            if (input.isVector())
                input.invalidateVector();
        }
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
            for (const auto edge : inputEdges_.at(inputName))
                if (!edge->sourceOutput().parent()->inputsAreValid())
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
    GraphDebug::increaseIndent();
    if (pullInputsOnRun_)
    {
        // Pull all input edges. If any are out-of-date and get re-set this will automatically unset upToDate_
        for (auto &[inputName, edges] : inputEdges_)
        {
            for (const auto edge : edges)
            {
                std::cout << std::format("{}Node[{}]::run() - pulling edge '{}'..\n", GraphDebug::indent(), name(),
                                         edge->definition().asString());

                switch (edge->pull())
                {
                    case (NodeConstants::ProcessResult::Failed):
                    case (NodeConstants::ProcessResult::InputsNotSatisfied):
                        return NodeConstants::ProcessResult::Failed;
                    case (NodeConstants::ProcessResult::Success):
                        std::cout << std::format("{}Node[{}]::run() - returned SUCCESS.\n", GraphDebug::indent(), name());
                        break;
                    case (NodeConstants::ProcessResult::Unchanged):
                        std::cout << std::format("{}Node[{}]::run() - returned UNCHANGED.\n", GraphDebug::indent(), name());
                        break;
                }
            }
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

    GraphDebug::decreaseIndent();
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
        // We accept one connection per input in the case of non-vector parameters, so if one already exists we complain.
        // Vector inputs are currently unbounded.
        if (inputEdges_.contains(edge->targetInput().name()))
        {
            // Already have input edges to this parameter, so check current size and type
            if (!inputEdges_.at(edge->targetInput().name()).empty())
            {
                if (edge->targetInput().nAllowedInputEdges() != ParameterBase::AllowedEdgeCount::AnyNumber)
                    return Messenger::error("Node '{}' refusing to accept Edge connecting to input '{}' as it already has the "
                                            "maximum permissible.\n",
                                            name(), edge->targetInput().name());
            }
        }

        // All good, so add the input to our list
        inputEdges_[edge->targetInput().name()].push_back(edge);

        // Adding an Edge to an input always invalidates the target
        invalidate();
    }
    else if (&edge->sourceNode() == this)
    {
        // We are the source node - add the outgoing edge to our list
        outputEdges_[edge->sourceOutput().name()].push_back(edge);
    }
    else
        return Messenger::error("Node '{}' is neither the source nor the target for the supplied Edge.\n", name());

    return true;
}

// Erase the specified edge from the given map, returning if it was found and erased
bool Node::eraseEdge(EdgeMap &map, Edge *edge)
{
    auto mapIt = std::find_if(map.begin(), map.end(),
                              [&](auto &edges)
                              {
                                  auto edgeIt = std::find(edges.second.begin(), edges.second.end(), edge);
                                  if (edgeIt != edges.second.end())
                                  {
                                      edges.second.erase(edgeIt);
                                      return true;
                                  }
                                  return edgeIt != edges.second.end();
                              });
    return mapIt != map.end();
}

// Unlink edge
void Node::unlinkEdge(Edge *edge)
{
    // If we are the Edge's targetNode_ then we should have its pointer in inputEdges_
    if (&edge->targetNode() == this)
    {
        if (eraseEdge(inputEdges_, edge))
            invalidate();
        else
            Messenger::error("Tried to unlink an incoming edge to target node '{}' which knew nothing about it.\n", name());
    }
    else if (&edge->sourceNode() == this)
    {
        // We are the source node for the edge...
        if (!eraseEdge(outputEdges_, edge))
            Messenger::error("Tried to unlink an outgoing edge from source node '{}' which knew nothing about it.\n", name());
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

// Mark incoming edges to the specified parameter as needing a re-pull
void Node::markIncomingEdgesForPull(const ParameterBase *toParameter) const
{
    if (!inputEdges_.contains(toParameter->name()))
        return;

    for (const auto edge : inputEdges_.at(toParameter->name()))
        edge->forceNextPull();
}

// Returns the node parent graph
Graph *Node::parentGraph() const { return parentGraph_; }

// Return the Dissolve reference
Dissolve &Node::dissolve() const { return parentGraph_->dissolve(); }

// Return the DissolveGraph reference
DissolveGraph *Node::dissolveGraph() { return parentGraph_->dissolveGraph(); }

/*
 * Data
 */

// Clear any local data
void Node::clearData() {}

// Return timing information (in seconds) for this Module
SampledDouble Node::timing() const { return timing_; }

/*
 * Serialisation
 */

// Express as a serialisable value
SerialisedValue Node::serialise() const
{
    SerialisedValue result, inputs, outputs, options;
    result["name"] = name();
    result["type"] = type();
    result["x"] = x;
    result["y"] = y;

    return result;
}

// Read values from a serialisable value
void Node::deserialise(const SerialisedValue &node)
{
    x = toml::find<int>(node, "x");
    y = toml::find<int>(node, "y");
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

// Express persistent data as a serialisable value
SerialisedValue Node::serialiseData() const
{
    SerialisedValue result;
    result["timing"] = timing_.serialise();

    for (auto &[key, serialisable] : serialisables_)
        if (serialisable->canSerialise())
            result[key] = serialisable->serialise();

    return result;
}

// Read persistent data from a serialisable value
void Node::deserialiseData(const SerialisedValue &node)
{
    // Obtain resolvable data // TODO
    std::map<std::string, const Species *> reachableSpecies;

    timing_.deserialise(node.at("timing"));

    // Read in defined serialisables if they exist
    for (auto &[key, serialisable] : serialisables_)
        if (node.contains(key))
        {
            // Deserialise the data
            serialisable->deserialise(node.at(key));

            // Resolve any named data
            serialisable->resolve(reachableSpecies);
        }
}
