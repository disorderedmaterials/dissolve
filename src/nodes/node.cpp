// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/node.h"
#include "base/sysFunc.h"

/*
 * Processing & Validity
 */

// Return version index for the node, bumped whenever result outputs change
int Node::versionIndex() const { return versionIndex_; }

// Invalidate the current node, resetting the version index
void Node::invalidate() { versionIndex_ = Node::InvalidVersion; }

// Check that all required inputs are present, and that all inputs are valid
bool Node::inputsAreValid() const
{
    for (auto &[inputName, parameter] : inputs_)
    {
        // Does this input have a link or links?
        if (inputLinks_.contains(inputName))
        {
            if (!inputLinks_.at(inputName).sourceOutput().parent()->inputsAreValid())
                return false;
        }
        else if (parameter->flags().isSet(ParameterBase::ParameterFlags::Required))
            return false;
    }

    return true;
}

// Run the node, retrieving linked inputs
Node::ProcessResult Node::run()
{
    auto result = ProcessResult::Success;
    // TODO Check our input links - if any are out-of-date we must retrieve new values
    auto nInputLinksChanged = 0;
    for (auto &[key, link] : inputLinks_)
    {
        //        // Ignore parameters that don't invalidate
        //        if (!link.sink().flags().isSet(ParameterBase::Invalidates))
        //            continue;
        //        // Update unsatisfied sources
        //        if (!(link.source().parent()->isSatisfied() || link.updateSource()))
        //            return Node::Readiness::MissingComponent;
    }

    // If input links have updated or we are currently flagged as invalid we must reprocess
    if (nInputLinksChanged > 0 || versionIndex_ == InvalidVersion)
    {
        result = process();
        if (result == ProcessResult::Success)
            ++versionIndex_;
    }

    return result;
}

// Perform processing
Node::ProcessResult Node::process() { return ProcessResult::Failed; }

/*
 * Inputs, Outputs, and Options
 */

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findInput(std::string_view name) const
{
    if (!inputs_.contains(name))
        return {};
    return inputs_.at(name);
}

// Return input parameters
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::inputs() { return inputs_; };

// Return named output parameter if it exists
std::shared_ptr<ParameterBase> Node::findOutput(std::string_view name) const
{
    if (!outputs_.contains(name))
        return {};
    return outputs_.at(name);
}

// Return output parameters
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::outputs() { return outputs_; };

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findOption(std::string_view name) const
{
    if (!options_.contains(name))
        return {};
    return options_.at(name);
}

// Return Options
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::options() { return options_; };

// Set the node parent graph
void Node::setParentGraph(Graph *parentGraph) { parentGraph_ = parentGraph; }

// Returns the node parent graph
Graph *Node::parentGraph() const { return parentGraph_; }

Node::LinkMap &Node::links() { return inputLinks_; }

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue Node::serialise() const
{
    SerialisedValue result, inputs, options;
    result["name"] = name();

    if (!inputs_.empty())
    {
        for (auto &[k, v] : inputs_)
            inputs[std::string(k)] = *v;
        result["inputs"] = inputs;
    }
    if (!options_.empty())
    {
        for (auto &[k, v] : options_)
            options[std::string(k)] = *v;
        result["options"] = options;
    }
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
