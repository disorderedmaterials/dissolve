// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/temperature.h"

TemperatureNode::TemperatureNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<Number>("Temperature", "Temperature (K)", temperature_);
    addInput<Configuration *>("Configuration", "Configuration to be modified", targetConfiguration_);
    addOutput<Configuration *>("Configuration", "Modified configuration", targetConfiguration_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view TemperatureNode::type() const { return "Temperature"; }

// Return short summary of the node's purpose
std::string_view TemperatureNode::summary() const { return "Sets a new temperature value for a given target configuration"; }

// Perform processing
NodeConstants::ProcessResult TemperatureNode::process()
{
    targetConfiguration_->setTemperature(temperature_.asDouble());
    return NodeConstants::ProcessResult::Success;
}