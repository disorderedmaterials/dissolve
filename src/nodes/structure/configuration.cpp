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

// Express state as a serialisable value
void ConfigurationNode::serialiseStateData(SerialisedValue &state) const
{
    state["temperature"] = configuration_.temperature();
    Serialisable::vector(configuration_.atoms(), "atoms", state,
                         [](const ConfigurationAtom &atom)
                         {
                             SerialisedValue result;
                             atom.r().serialise("r", result);
                             return result["r"];
                         });
}

// Read state data from a serialisable value
bool ConfigurationNode::deserialiseStateData(const SerialisedValue &state)
{
    if (state.contains("temperature"))
        configuration_.setTemperature(state.at("temperature").as_floating());
    else
        return false;
    if (state.contains("atoms"))
    {
        auto pos = Deserialisable::vector<Vector3>(state.at("atoms"));
        auto &atoms = configuration_.atoms();
        for (auto i = 0; i < atoms.size(); ++i)
            atoms[i].setR(pos[i]);
    }
    else
        return false;
    return true;
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ConfigurationNode::process() { return NodeConstants::ProcessResult::Unchanged; }
