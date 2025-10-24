// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/inputs.h"

InputsNode::InputsNode(Graph *parentGraph) : Node(parentGraph) { pullInputsOnRun_ = false; }

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view InputsNode::type() const { return "Inputs"; }

// Return short summary of the node's purpose
std::string_view InputsNode::summary() const { return "Maps graph inputs to local outputs"; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult InputsNode::process() { return NodeConstants::ProcessResult::Success; }

/*
 * Inputs, Outputs, and Options
 */

// Add a proxy output, creating a loopback input for it at the same time
bool InputsNode::addProxy(std::shared_ptr<ParameterBase> &output)
{
    // Own the output end of the proxy connection
    if (!ownParameter(output, true))
        return false;

    // Create a loopback input of the same name
    auto param = inputs_.emplace(std::make_pair(output->name(), output)).first->second;
    param->setFlags(ParameterBase::ParameterFlags::Input);

    return true;
}

/*
 * Serialisation
 */

// Is it appropriate to bother serialising this node?
bool InputsNode::shouldSerialise() const { return false; }

// Express as a serialisable value
SerialisedValue InputsNode::serialise() const { return {}; }

// Read values from a serialisable value
void InputsNode::deserialise(const SerialisedValue &node) {};
