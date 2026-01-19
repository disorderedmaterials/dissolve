#include "nodes/multiply.h"

MultiplyNode::MultiplyNode(Graph *parentGraph) : Node(parentGraph)
{
    addSerialisableInput<Number>("X", "First factor to the multiplication", x_);
    addSerialisableInput<Number>("Y", "Second factor to the multiplication", y_);
    addOutput<Number>("Result", "The product of the two factors", result_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view MultiplyNode::type() const { return "Multiply"; }

// Return short summary of the node's purpose
std::string_view MultiplyNode::summary() const { return "Performs multiplication of factors X and Y"; }

// Perform processing
NodeConstants::ProcessResult MultiplyNode::process()
{
    result_ = x_ * y_;

    return NodeConstants::ProcessResult::Success;
}
