#include "vec3Decomposition.h"

Vec3DecompositionNode::Vec3DecompositionNode(Graph *parentGraph) : Node(parentGraph)
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

std::string_view Vec3DecompositionNode::type() const { return "Vector3Decomposition"; }

std::string_view Vec3DecompositionNode::summary() const { return "Decompose a 3-vector into x, y, and z components"; }

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult Vec3DecompositionNode::process()
{
    x_ = inputVector_.x;
    y_ = inputVector_.y;
    z_ = inputVector_.z;

    return NodeConstants::ProcessResult::Success;
}
