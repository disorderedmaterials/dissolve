#include "nodes/subtract.h"

SubtractNode::SubtractNode()
{
    addInput<Number>("A", "First operand to the subtraction", a_);
    addInput<Number>("B", "Second operand to the subtraction, subtracted from A", b_);
    addOutput<Number>("Difference", "The difference of the operands", result_);
}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view SubtractNode::type() const { return "Subtract"; }

// Return short summary of the node's purpose
std::string_view SubtractNode::summary() const { return "Performs the subtraction A - B"; }

// Perform processing
NodeConstants::ProcessResult SubtractNode::process()
{
    result_ = a_ - b_;

    return NodeConstants::ProcessResult::Success;
}
