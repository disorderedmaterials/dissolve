// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/configuration.h"

ConfigurationNode::ConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Outputs
    addPointerOutput<Configuration>("Configuration", "Configuration object", configuration_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ConfigurationNode::type() const { return "Configuration"; }

// Return short summary of the node's purpose
std::string_view ConfigurationNode::summary() const { return "A Configuration."; }

/*
 * Data
 */

// Return configuration object
Configuration &ConfigurationNode::configuration() { return configuration_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ConfigurationNode::process() { return NodeConstants::ProcessResult::Failed; }
