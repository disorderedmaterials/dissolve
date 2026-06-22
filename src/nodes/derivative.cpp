#include "derivative.h"
#include "math/derivative.h"

DerivativeNode::DerivativeNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Data1D>("Data1D", "Input 1D data series", inputData_);
    addOutput<Data1D>("Result", "The elementwise derivative of the input", derivative_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view DerivativeNode::type() const { return "Derivative"; }

// Return short summary of the node's purpose
std::string_view DerivativeNode::summary() const { return "Computes the derivative of a 1D data series"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult DerivativeNode::process()
{
    derivative_ = Derivative::derivative(inputData_);

    return NodeConstants::ProcessResult::Success;
}
