// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/outputs.h"
#include "nodes/graph.h"

OutputsNode::OutputsNode(Graph *parentGraph) : Node(parentGraph) {}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view OutputsNode::type() const { return "Outputs"; }

// Return short summary of the node's purpose
std::string_view OutputsNode::summary() const { return "Maps local inputs to graph outputs"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult OutputsNode::process() { return NodeConstants::ProcessResult::Success; }

// Flag that the node data needs to be updated
void OutputsNode::setUpdateRequired()
{
    // If already flagged then do nothing
    if (!isUpToDate())
        return;

    // Need to flag parent graph since we represent its outputs
    parentGraph_->setUpdateRequired();

    // Call base class function to set flag and propagate through outputs (not there there should be any)
    Node::setUpdateRequired();
}

/*
 * Serialisation
 */

// Is it appropriate to bother serialising this node?
bool OutputsNode::shouldSerialise() const { return false; }

// Express as a serialisable value
void OutputsNode::serialise(std::string tag, SerialisedValue &target) const {}

// Read values from a serialisable value
void OutputsNode::deserialise(const SerialisedValue &node) {};
