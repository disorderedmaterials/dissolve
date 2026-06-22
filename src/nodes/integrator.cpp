#include "integrator.h"

Integrator1DNode::Integrator1DNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Data1D>("Data1D", "Input 1D data series", inputData_);

    // Options
    addOption("Method", "Method to use for integration", method_);

    // Outputs
    addOutput<Number>("Result", "The integration of the input data series", integral_);
}

/*
 * Definition
 */

std::string_view Integrator1DNode::type() const { return "Integrator"; }

std::string_view Integrator1DNode::summary() const { return "Computes the integral for a 1D data series"; }

/*
 * Data
 */

// Return enum options for form
EnumOptions<Integrator1DNode::IntegratorMethod> Integrator1DNode::integratorMethods()
{
    return EnumOptions<Integrator1DNode::IntegratorMethod>(
        "IntegratorMethod", {
                                {Integrator1DNode::IntegratorMethod::Trapezoidal, "Trapezoidal"},
                                {Integrator1DNode::IntegratorMethod::AbsoluteTrapezoidal, "AbsoluteTrapezoidal"},
                                {Integrator1DNode::IntegratorMethod::Sum, "Sum"},
                                {Integrator1DNode::IntegratorMethod::AbsoluteSum, "AbsoluteSum"},
                                {Integrator1DNode::IntegratorMethod::SumOfSquares, "SumOfSquares"},
                            });
}
EnumOptions<Integrator1DNode::IntegratorMethod> getEnumOptions(Integrator1DNode::IntegratorMethod)
{
    return Integrator1DNode::integratorMethods();
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult Integrator1DNode::process()
{
    switch (method_)
    {
        case (IntegratorMethod::Trapezoidal):
            integral_ = Integrator::trapezoid(inputData_);
            break;
        case (IntegratorMethod::AbsoluteTrapezoidal):
            integral_ = Integrator::absTrapezoid(inputData_);
            break;
        case (IntegratorMethod::Sum):
            integral_ = Integrator::sum(inputData_);
            break;
        case (IntegratorMethod::AbsoluteSum):
            integral_ = Integrator::absSum(inputData_);
            break;
        case (IntegratorMethod::SumOfSquares):
            integral_ = Integrator::sumOfSquares(inputData_);
            break;
        default:
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
