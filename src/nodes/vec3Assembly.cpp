#include "vec3Assembly.h"

Vec3AssemblyNode::Vec3AssemblyNode()
{
    addInput<double>("x", "The x value of the assembled vector", x_);
    addInput<double>("y", "The y value of the assembled vector", y_);
    addInput<double>("z", "The z value of the assembled vector", z_);
    addOutput<Vec3<double>>("outputVector_", "The assembed vector", outputVector_);
}

std::string_view Vec3AssemblyNode::name() const { return "Vector3 Assembly"; }

std::string_view Vec3AssemblyNode::summary() const { return "Assemble a 3-vector from x, y, and z values."; }

// Run main processing
Module::ExecutionResult Vec3AssemblyNode::process(ModuleContext &moduleContext)
{
    outputVector_.set(x_, y_, z_);

    return Module::ExecutionResult::Success;
}
