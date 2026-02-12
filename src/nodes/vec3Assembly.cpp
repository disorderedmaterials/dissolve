#include "vec3Assembly.h"

Vec3AssemblyNode::Vec3AssemblyNode(Graph *parentGraph) : Node(parentGraph)
{
    addSerialisableInput<double>("X", "The x value of the assembled vector", x_);
    addSerialisableInput<double>("Y", "The y value of the assembled vector", y_);
    addSerialisableInput<double>("Z", "The z value of the assembled vector", z_);
    addOutput<Vector3>("V", "The assembled vector", outputVector_);
}

std::string_view Vec3AssemblyNode::type() const { return "Vector3 Assembly"; }

std::string_view Vec3AssemblyNode::summary() const { return "Assemble a 3-vector from x, y, and z values."; }

// Run main processing
NodeConstants::ProcessResult Vec3AssemblyNode::process()
{
    outputVector_.set(x_, y_, z_);

    return NodeConstants::ProcessResult::Success;
}
