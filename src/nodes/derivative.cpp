#include "derivative.h"
#include "math/derivative.h"

DerivativeNode::DerivativeNode()
{ 
	addInput<Data1D>("InputData", "Input 1D data series", inputData_);
}

std::string_view DerivativeNode::name() { return "Derivative"; }

std::string_view DerivativeNode::summary(){ return "Computes the derivate of a 1D data series"; }

Module::ExecutionResult DerivativeNode::process(ModuleContext& moduleContext)
{
    derivative_ = Derivative::derivative(inputData_);

	return ExecutionResult::Success;
}