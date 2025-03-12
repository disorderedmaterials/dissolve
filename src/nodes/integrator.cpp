#include "integrator.h"

Integrator1DNode::Integrator1DNode()
{
    addInput<Data1D>("InputData", "Input 1D data series", inputData_);
    addInput<std::string_view>("IntegrationMethod", "Method to use for integration", type_);
}

// Return enum options for form
static EnumOptions<Integrator1DNode::Method> Integrator1DNode::types() 
{ 
    return EnumOptions<Integrator1DNode::Method>("Integrator1DNode", {
            {Integrator1DNode::Method::Trapezoidal, "Trapezoidal"},
            {Integrator1DNode::Method::AbsoluteTrapezoidal, "AbsoluteTrapezoidal"},
            {Integrator1DNode::Method::Sum, "Sum"},
            {Integrator1DNode::Method::AbsoluteSum, "AbsoluteSum"},
            {Integrator1DNode::Method::SumOfSquares, "SumOfSquares"},
        }
    ); 
}

std::string_view Integrator1DNode::name() { return "Integrator"; }

std::string_view Integrator1DNode::summary() { return "Computes the integral for a 1D data series"; }

// Run main processing
Module::ExecutionResult Integrator1DNode::process(ModuleContext &moduleContext)
{
    auto typeEnum = types().enumeration(type_);

    switch (typeEnum)
    {
        case static_cast<int>(Method::Trapezoidal):
            integral_ = Integrator::trapezoid(inputData_);

        case static_cast<int>(Method::AbsoluteTrapezoidal):
            integral_ = Integrator::absTrapezoid(inputData_);

        case static_cast<int>(Method::Sum):
            integral_ = Integrator::sum(inputData_);

        case static_cast<int>(Method::AbsoluteSum):
            integral_ = Integrator::absSum(inputData_);

        case static_cast<int>(Method::SumOfSquares):
            integral_ = Integrator::sumOfSquares(inputData_);

        default:
            return ExecutionResult::Failure;
    }

    return ExecutionResult::Success;
}
