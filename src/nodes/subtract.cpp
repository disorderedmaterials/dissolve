#include "nodes/subtract.h"

SubtractNode::SubtractNode(Graph *parentGraph) : Node(parentGraph)
{
    addSerialisableInput<Number>("X", "First operand to the subtraction", x_);
    addSerialisableInput<Number>("Y", "Second operand to the subtraction, subtracted from X", y_);
    addOutput<Number>("Result", "The difference of the operands", result_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view SubtractNode::type() const { return "Subtract"; }

// Return short summary of the node's purpose
std::string_view SubtractNode::summary() const { return "Performs the subtraction X - Y"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult SubtractNode::process()
{
    result_ = x_ - y_;

    return NodeConstants::ProcessResult::Success;
}
