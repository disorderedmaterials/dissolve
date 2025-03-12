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

/*
 * Inputs
 */

// Return input parameters
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::parameters() { return inputs_; };

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
void Node::setParentGraph(Graph* parentGraph) { parentGraph_ = parentGraph; }

// Returns the node parent graph
Graph *Node::parentGraph() const { return parentGraph_; }

// Tell node to recalculate results
void Node::invalidate() { satisfied_ = false; }

// Tell node that results are up to date
void Node::validate() { satisfied_ = true; }

Node::LinkMap &Node::links() { return inputLinks_; }
