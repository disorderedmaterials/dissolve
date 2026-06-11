// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/configuration.h"

ConfigurationNode::ConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    addPointerOutput<Configuration>("Configuration", "Configuration object", configuration_);
}

std::string_view ConfigurationNode::type() const { return "Configuration"; }

std::string_view ConfigurationNode::summary() const { return "Produce an empty atomic configuration."; }

// Return configuration object
const Configuration &ConfigurationNode::configuration() const { return configuration_; }

NodeConstants::ProcessResult ConfigurationNode::process() { return NodeConstants::ProcessResult::Unchanged; }
