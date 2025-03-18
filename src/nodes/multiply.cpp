#include "nodes/multiply.h"

MultiplyNode::MultiplyNode()
{
    addInput<NodeNumber>("A", "First factor to the multiplication", a_);
    addInput<NodeNumber>("B", "Second factor to the multiplication", b_);
    addOutput<NodeNumber>("Product", "The product of the two factors", result_);
}

 /*
 * Definition (Virtuals)
 */

// Return short name of the node
std::string_view MultiplyNode::name() const
{ 
    return "Multiply"; 
}


// Return short summary of the node's purpose
std::string_view MultiplyNode::summary() const
{ 
    return "Performs multiplication of factors A and B";
}

// Perform processing
Node::ProcessResult MultiplyNode::process()
{
    if (!isSatisfied())
        return ProcessResult::InputsNotSatisfied;
 
    result_ = a_ * b_;

    return ProcessResult::Success;
}