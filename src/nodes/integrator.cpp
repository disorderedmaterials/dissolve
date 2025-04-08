#include "integrator.h"

Integrator1DNode::Integrator1DNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Data1D>("Data1D", "Input 1D data series", inputData_);
    addInput<std::string_view>("Method", "Method to use for integration", type_);
    addOutput<Number>("Result", "The integration of the input data series", integral_);
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

std::string_view Integrator1DNode::type() const { return "Integrator"; }

std::string_view Integrator1DNode::summary() const { return "Computes the integral for a 1D data series"; }

// Run main processing
NodeConstants::ProcessResult Integrator1DNode::process()
{
    switch (types().enumeration(type_))
    {
        case Method::Trapezoidal:
            integral_ = Integrator::trapezoid(inputData_);
            break;
        case Method::AbsoluteTrapezoidal:
            integral_ = Integrator::absTrapezoid(inputData_);
            break;
        case Method::Sum:
            integral_ = Integrator::sum(inputData_);
            break;
        case Method::AbsoluteSum:
            integral_ = Integrator::absSum(inputData_);
            break;
        case Method::SumOfSquares:
            integral_ = Integrator::sumOfSquares(inputData_);
            break;
        default:
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
