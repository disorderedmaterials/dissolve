// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/node.h"
#include "base/sysFunc.h"

/*
 * Inputs
 */

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findParameter(std::string_view name) const
{
    if (!inputs_.contains(name))
        return {};
    return inputs_.at(name);
}

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findOption(std::string_view name) const
{
    if (!options_.contains(name))
        return {};
    return options_.at(name);
}

/*
 * Inputs
 */

// Return input parameters
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::parameters() { return inputs_; };

// Return Options
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::options() { return options_; };

// Prepare for processing
Node::Readiness Node::preprocess()
{
    for (auto &[key, link] : inputLinks_)
    {
        // Ignore parameters that don't invalidate
        if (!link.sink().flags().isSet(ParameterBase::Invalidates))
            continue;
        // Update unsatisfied sources
        if (!(link.source().parent()->isSatisfied() || link.updateSource()))
            return Node::Readiness::MissingComponent;
    }
    return Node::Readiness::Ready;
}

// Confirm that node data is up to date
bool Node::isSatisfied()
{
    if (satisfied_)
        return true;
    for (auto &[name, link] : inputLinks_)
        if (!link.source().parent()->isSatisfied())
            satisfied_ = false;
    return satisfied_;
}

// Set the node parent graph
void Node::setParentGraph(Graph *parentGraph) { parentGraph_ = parentGraph; }

// Returns the node parent graph
Graph *Node::parentGraph() const { return parentGraph_; }

// Tell node to recalculate results
void Node::invalidate() { satisfied_ = false; }

// Tell node that results are up to date
void Node::validate() { satisfied_ = true; }

Node::LinkMap &Node::links() { return inputLinks_; }

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
