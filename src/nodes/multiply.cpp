#include "nodes/multiply.h"

MultiplyNode::MultiplyNode(Graph *parentGraph) : Node(parentGraph)
{
    addSerialisableInput<Number>("X", "First factor to the multiplication", a_);
    addSerialisableInput<Number>("Y", "Second factor to the multiplication", b_);
    addOutput<Number>("Result", "The product of the two factors", result_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view MultiplyNode::type() const { return "Multiply"; }

// Return short summary of the node's purpose
std::string_view MultiplyNode::summary() const { return "Performs multiplication of factors A and B"; }

// Perform processing
NodeConstants::ProcessResult MultiplyNode::process()
{
    result_ = a_ * b_;

    return NodeConstants::ProcessResult::Success;
}
