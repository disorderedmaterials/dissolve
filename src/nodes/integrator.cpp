#include "integrator.h"

Integrator1DNode::Integrator1DNode()
{
    addInput<Data1D>("InputData", "Input 1D data series", inputData_);
    addInput<std::string_view>("IntegrationMethod", "Method to use for integration", type_);
    addOutput<double>("Integral", "The integration of the input data series", integral_);
}

// Return enum options for form
EnumOptions<Integrator1DNode::Method> Integrator1DNode::types()
{
    return EnumOptions<Integrator1DNode::Method>("Integrator1DNode",
                                                 {
                                                     {Integrator1DNode::Method::Trapezoidal, "Trapezoidal"},
                                                     {Integrator1DNode::Method::AbsoluteTrapezoidal, "AbsoluteTrapezoidal"},
                                                     {Integrator1DNode::Method::Sum, "Sum"},
                                                     {Integrator1DNode::Method::AbsoluteSum, "AbsoluteSum"},
                                                     {Integrator1DNode::Method::SumOfSquares, "SumOfSquares"},
                                                 });
}

std::string_view Integrator1DNode::name() const { return "Integrator"; }

std::string_view Integrator1DNode::summary() const { return "Computes the integral for a 1D data series"; }

// Run main processing
Module::ExecutionResult Integrator1DNode::process(ModuleContext &moduleContext)
{
    auto typeEnum = types().enumeration(type_);

    switch (typeEnum)
    {
        case Method::Trapezoidal:
            integral_ = Integrator::trapezoid(inputData_);

        case Method::AbsoluteTrapezoidal:
            integral_ = Integrator::absTrapezoid(inputData_);

        case Method::Sum:
            integral_ = Integrator::sum(inputData_);

        case Method::AbsoluteSum:
            integral_ = Integrator::absSum(inputData_);

        case Method::SumOfSquares:
            integral_ = Integrator::sumOfSquares(inputData_);

        default:
            return Module::ExecutionResult::Failed;
    }

    validate();
    return Module::ExecutionResult::Success;
}
