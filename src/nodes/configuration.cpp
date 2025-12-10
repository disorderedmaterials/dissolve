// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/configuration.h"

ConfigurationNode::ConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<Number>("Temperature", "Configuration temperature (K)", temperature_);
    addPointerOutput<Configuration>("Configuration", "Configuration object", configuration_);
}

std::string_view ConfigurationNode::type() const { return "Configuration"; }

std::string_view ConfigurationNode::summary() const { return "Produce an empty atomic configuration."; }

NodeConstants::ProcessResult ConfigurationNode::process()
{
    configuration_.setTemperature(temperature_.asInteger());
    return NodeConstants::ProcessResult::Unchanged;
}
