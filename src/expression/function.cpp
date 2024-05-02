// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/function.h"
#include "math/constants.h"
#include "math/mathFunc.h"

// Return enum options for NodeTypes
EnumOptions<ExpressionFunctionNode::InternalFunction> ExpressionFunctionNode::internalFunctions()
{
    return EnumOptions<ExpressionFunctionNode::InternalFunction>("InternalFunction", {{AbsFunction, "abs", 1},
                                                                                      {ACosFunction, "acos", 1},
                                                                                      {ASinFunction, "asin", 1},
                                                                                      {ATanFunction, "atan", 1},
                                                                                      {CosFunction, "cos", 1},
                                                                                      {ExpFunction, "exp", 1},
                                                                                      {LnFunction, "ln", 1},
                                                                                      {LogFunction, "log", 1},
                                                                                      {PiFunction, "pi"},
                                                                                      {RandIntRangeFunction, "randomi", 1},
                                                                                      {SinFunction, "sin", 1},
                                                                                      {SqrtFunction, "sqrt", 1},
                                                                                      {TanFunction, "tan", 1},
                                                                                      {ToDegreesFunction, "toDeg", 1},
                                                                                      {ToRadiansFunction, "toRad", 1},
                                                                                      {TwoPiFunction, "twopi"},
                                                                                      {UnitRandomFunction, "random"}});
}

ExpressionFunctionNode::ExpressionFunctionNode(InternalFunction func) : ExpressionNode(), function_(func) {}

/*
 * Nodes
 */

// Duplicate this node and its contents
std::shared_ptr<ExpressionNode> ExpressionFunctionNode::duplicate()
{
    auto node = std::make_shared<ExpressionFunctionNode>(function_);

    for (const auto &child : children_)
        node->addChild(child->duplicate());

    return node;
}

/*
 * Evaluation
 */

// Evaluate node
std::optional<ExpressionValue> ExpressionFunctionNode::evaluate() const
{
    // Number of required child nodes depends on the function
    const auto nArgs = internalFunctions().minArgs(function_);
    if (children_.size() != nArgs.value_or(0))
        return std::nullopt;

    // Evaluate the arguments
    std::vector<ExpressionValue> args;
    for (auto n = 0; n < nArgs; ++n)
    {
        auto optArg = children_[n]->evaluate();
        if (!optArg)
            return std::nullopt;
        args.emplace_back(*optArg);
    }

    // Evaluate the function
    ExpressionValue result;
    switch (function_)
    {
        case (AbsFunction):
            if (args[0].isInteger())
                result = abs(args[0].asInteger());
            else
                result = fabs(args[0].asDouble());
            break;
        case (ACosFunction):
            result = acos(args[0].asDouble());
            break;
        case (ASinFunction):
            result = asin(args[0].asDouble());
            break;
        case (ATanFunction):
            result = atan(args[0].asDouble());
            break;
        case (CosFunction):
            result = cos(args[0].asDouble());
            break;
        case (ExpFunction):
            result = exp(args[0].asDouble());
            break;
        case (LnFunction):
            result = log(args[0].asDouble());
            break;
        case (LogFunction):
            result = log10(args[0].asDouble());
            break;
        case (PiFunction):
            result = M_PI;
            break;
        case (RandIntRangeFunction):
            result = DissolveMath::randomi(args[0].asInteger());
            break;
        case (SinFunction):
            result = sin(args[0].asDouble());
            break;
        case (SqrtFunction):
            result = sqrt(args[0].asDouble());
            break;
        case (TanFunction):
            result = tan(args[0].asDouble());
            break;
        case (ToDegreesFunction):
            result = args[0].asDouble() * DEGRAD;
            break;
        case (ToRadiansFunction):
            result = args[0].asDouble() / DEGRAD;
            break;
        case (TwoPiFunction):
            result = 2.0 * M_PI;
            break;
        case (UnitRandomFunction):
            result = DissolveMath::random();
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Expression function '{}' has not been implemented.\n", internalFunctions().keyword(function_))));
    }

    return result;
}

// Return string representation of node
std::string ExpressionFunctionNode::asString() const
{
    // Number of required child nodes depends on the function
    const auto nArgs = internalFunctions().minArgs(function_);
    if (children_.size() != nArgs.value_or(0))
        return "";

    if (!nArgs)
        return fmt::format("{}()", internalFunctions().keyword(function_));

    // Evaluate the arguments
    std::vector<std::string> args;
    for (auto n = 0; n < nArgs; ++n)
    {
        auto optArg = children_[n]->asString();
        if (optArg.empty())
            return "";
        args.emplace_back(optArg);
    }

    return fmt::format("{}({})", internalFunctions().keyword(function_), args[0]);
}
