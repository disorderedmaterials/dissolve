// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/node.h"
#include "base/sysFunc.h"

/*
 * Inputs
 */

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findInput(std::string_view name) const
{
    if (!inputs_.contains(name))
        return {};
    return inputs_.at(name);
}

/*
 * Inputs
 */

// Return input parameters
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::inputs() { return inputs_; };

// Prepare for processing
bool Node::preprocess()
{
    for (auto &[key, link] : links_)
    {
        // Ignore parameters that don't invalidate
        if (!link.sink().flags().isSet(ParameterBase::Invalidates))
            continue;
        // Update unsatisfied sources
        if (!(link.source().parent()->isSatisfied() || link.updateSource()))
            return false;
    }
    return true;
}

// Confirm that node data is up to date
bool Node::isSatisfied()
{
    if (satisfied_)
        return true;
    for (auto &[name, link] : links_)
        if (!link.source().parent()->isSatisfied())
            satisfied_ = false;
    return satisfied_;
}

// Tell node to recalculate results
void Node::invalidate() { satisfied_ = false; }

// Tell node that results are up to date
void Node::validate() { satisfied_ = true; }
