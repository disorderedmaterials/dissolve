#include "vec3Decomposition.h"

Vec3DecompositionNode::Vec3DecompositionNode()
{
    addInput<Vec3<double>>("InputVector", "The 3-dimensional vector to be decomposed", inputVector_);
    addOutput<double>("X", "The first component of the vector", x_);
    addOutput<double>("Y", "The first component of the vector", y_);
    addOutput<double>("Z", "The first component of the vector", z_);
}

std::string_view Vec3DecompositionNode::type() const { return "Vector3 Decomposition"; }

std::string_view Vec3DecompositionNode::summary() const { return "Decompose a 3-vector into x, y, and z components."; }

// Run main processing
Module::ExecutionResult Vec3DecompositionNode::process(ModuleContext &moduleContext)
{
    x_ = inputVector_.x;
    y_ = inputVector_.y;
    z_ = inputVector_.z;

    return Module::ExecutionResult::Success;
}
