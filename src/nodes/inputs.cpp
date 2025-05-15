// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/inputs.h"

InputsNode::InputsNode(Graph *parentGraph) : Node(parentGraph) {}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view InputsNode::type() const { return "Inputs"; }

// Return short summary of the node's purpose
std::string_view InputsNode::summary() const { return "Maps graph inputs to local outputs"; }

/*
 * Parameter Maps
 */

// Create input and map to output
std::shared_ptr<ParameterBase> InputsNode::createMappedInput(std::string_view name, std::type_index typeIndex)
{
    // Create an intermediate object with the correct type
    std::shared_ptr<ParameterBase> parameterBase;
    std::shared_ptr<ParameterHolderBase> proxyBase;
    // TODO Convert to Factory
    if (typeIndex == std::type_index(typeid(Number)))
    {
        auto proxy = std::make_shared<ParameterHolder<Number>>();
        parameterBase = addInput(name, "", proxy->data);
        proxyBase = proxy;
    }

    parameterHolders_.emplace_back(proxyBase);
    return parameterBase;
}

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue InputsNode::serialise() const { return {}; }

// Read values from a serialisable value
void InputsNode::deserialise(const SerialisedValue &node){};
