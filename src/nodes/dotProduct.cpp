// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "dotProduct.h"

DotProductNode::DotProductNode(Graph *parentGraph) : Node(parentGraph)
{
    addSerialisableInput<Vector3>("U", "Vector dot product factor", u_);
    addSerialisableInput<Vector3>("V", "Vector dot product factor", v_);
    addOutput<Number>("Result", "The inner product of the vectors", dotProduct_);
}

/*
 * Definition
 */

std::string_view DotProductNode::type() const { return "Dot Product"; }

std::string_view DotProductNode::summary() const { return "Compute the dot product of vectors u and v"; }

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult DotProductNode::process()
{
    dotProduct_ = u_.dp(v_);

    return NodeConstants::ProcessResult::Success;
}
