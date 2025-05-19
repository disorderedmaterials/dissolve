// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameterMapping.h"

ParameterMappingNode::ParameterMappingNode(Graph *parentGraph, bool mapsOutputs) : Node(parentGraph), mapsOutputs_{mapsOutputs}
{
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view ParameterMappingNode::type() const { return "Inputs"; }

// Return short summary of the node's purpose
std::string_view ParameterMappingNode::summary() const { return "Maps graph inputs to local outputs"; }

/*
 * Data
 */

// Return whether the node maps its inputs to Graph outputs
bool ParameterMappingNode::mapsOutputs() const { return mapsOutputs_; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult ParameterMappingNode::process()
{
    // If this is an input mapper
    return NodeConstants::ProcessResult::Success;
}

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue ParameterMappingNode::serialise() const { return {}; }

// Read values from a serialisable value
void ParameterMappingNode::deserialise(const SerialisedValue &node){};
