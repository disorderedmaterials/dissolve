// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/vector3Decompose.h"

Vector3DecomposeNode::Vector3DecomposeNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addSerialisableInput<Vector3>("V", "The 3-dimensional vector to be decomposed", inputVector_);

    // Outputs
    addOutput<Number>("X", "The x component of the vector", x_);
    addOutput<Number>("Y", "The y component of the vector", y_);
    addOutput<Number>("Z", "The z component of the vector", z_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view Vector3DecomposeNode::type() const { return "Vector3Decompose"; }

// Return short summary of the node's purpose
std::string_view Vector3DecomposeNode::summary() const { return "Decompose a 3-vector into x, y, and z components"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult Vector3DecomposeNode::process()
{
    x_ = inputVector_.x;
    y_ = inputVector_.y;
    z_ = inputVector_.z;

    return NodeConstants::ProcessResult::Success;
}
