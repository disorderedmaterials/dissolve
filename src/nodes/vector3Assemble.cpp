// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/vector3Assemble.h"

Vector3AssembleNode::Vector3AssembleNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addSerialisableInput<double>("X", "The x value of the assembled vector", x_);
    addSerialisableInput<double>("Y", "The y value of the assembled vector", y_);
    addSerialisableInput<double>("Z", "The z value of the assembled vector", z_);

    // Outputs
    addOutput<Vector3>("V", "The assembled vector", outputVector_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view Vector3AssembleNode::type() const { return "Vector3Assemble"; }

// Return short summary of the node's purpose
std::string_view Vector3AssembleNode::summary() const { return "Assemble a 3-vector from x, y, and z values"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult Vector3AssembleNode::process()
{
    outputVector_.set(x_, y_, z_);

    return NodeConstants::ProcessResult::Success;
}
