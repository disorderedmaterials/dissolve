// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/structure/configuration.h"
#include "base/serialiser.h"
#include "base/serialiserLibrary.h"
#include <algorithm>

ConfigurationNode::ConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Outputs
    addPointerOutput<Configuration>("Configuration", "Configuration object", configuration_);

    // State
    // TODO DISSOLVE2 - We don't have access to the member variables right now, but we will after we move Configuration to
    // ConfigurationNode addStateData("Temperature", configuration_.temperature()); addStateData("Molecules",
    // configuration_.molecules());
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
NodeConstants::ProcessResult ConfigurationNode::process() { return NodeConstants::ProcessResult::Unchanged; }
