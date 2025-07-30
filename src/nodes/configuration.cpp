// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/configuration.h"
#include "nodes/dissolve.h"

ConfigurationNode::ConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    addPointerOutput<Configuration>("Configuration", "Configuration object", configuration_);
}

std::string_view ConfigurationNode::type() const { return "Configuration"; }

std::string_view ConfigurationNode::summary() const { return "Produce an empty atomic configuration."; }

NodeConstants::ProcessResult ConfigurationNode::process()
{
    auto atomTypes = DissolveGraph::atomTypes(&configuration_);

    if (!DissolveGraph::updatePairPotentials(dissolve(), atomTypes))
        return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Unchanged;
}
