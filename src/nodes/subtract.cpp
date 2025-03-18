#include "nodes/subtract.h"

SubtractNode::SubtractNode()
{
    addInput<NodeNumber>("A", "First operand to the subtraction", a_);
    addInput<NodeNumber>("B", "Second operand to the subtraction, subtracted from A", b_);
    addOutput<NodeNumber>("Difference", "The difference of the operands", result_);
}

/*
 * Definition (Virtuals)
 */

// Return short name of the node
std::string_view SubtractNode::name() const
{ 
    return "Subtract";
}

// Return short summary of the node's purpose
std::string_view SubtractNode::summary() const
{ 
    return "Performs the subtraction A - B";
}

// Perform processing
Node::ProcessResult SubtractNode::process()
{
    if (!isSatisfied())
        return ProcessResult::InputsNotSatisfied;

    result_ = std::minus<T>(a_, b_);

    return ProcessResult::Success;
}