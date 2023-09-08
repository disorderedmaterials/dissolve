// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "expression/ExpressionVisitor.h"
#include "data/ff/ff.h"
#include "expression/ExpressionErrorListeners.h"
#include "expression/binary.h"
#include "expression/function.h"
#include "expression/number.h"
#include "expression/reference.h"
#include "expression/unary.h"
#include "expression/variable.h"
#include "templates/optionalRef.h"

/*
 * Creation Entry Point
 */

// Return the topmost context in the stack
std::shared_ptr<ExpressionNode> ExpressionVisitor::currentContext() const
{
    assert(contextStack_.size() != 0);

    return contextStack_.back();
}

// Construct description within supplied object, from given tree
void ExpressionVisitor::create(
    Expression &expr, ExpressionParser::ExpressionContext *tree,
    const std::vector<std::shared_ptr<ExpressionVariable>> &localVariables,
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> externalVariables)
{
    expression_ = &expr;
    localVariables_ = localVariables;
    externalVariables_ = externalVariables;

    // Add the definition's root node as the topmost context in the stack
    contextStack_.push_back(expression_->rootNode());

    // Traverse the AST
    visitChildren(tree);
}

/*
 * Visitor Overrides
 */

antlrcpp::Any ExpressionVisitor::visitParentheses(ExpressionParser::ParenthesesContext *ctx)
{
    auto node = std::make_shared<ExpressionRootNode>(true);

    currentContext()->addChild(node);

    contextStack_.push_back(node);

    auto result = visitChildren(ctx);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any ExpressionVisitor::visitUnaryMinus(ExpressionParser::UnaryMinusContext *ctx)
{
    auto node = std::make_shared<ExpressionUnaryOperatorNode>(ExpressionUnaryOperatorNode::OperatorNegate);

    currentContext()->addChild(node);

    contextStack_.push_back(node);

    auto result = visitChildren(ctx);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any ExpressionVisitor::visitPower(ExpressionParser::PowerContext *ctx)
{
    auto node = std::make_shared<ExpressionBinaryOperatorNode>(ExpressionBinaryOperatorNode::OperatorPow);

    currentContext()->addChild(node);

    contextStack_.push_back(node);

    auto result = visitChildren(ctx);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any ExpressionVisitor::visitMultiplyDivide(ExpressionParser::MultiplyDivideContext *ctx)
{
    auto node = std::make_shared<ExpressionBinaryOperatorNode>(ctx->Multiply() ? ExpressionBinaryOperatorNode::OperatorMultiply
                                                                               : ExpressionBinaryOperatorNode::OperatorDivide);

    currentContext()->addChild(node);

    contextStack_.push_back(node);

    auto result = visitChildren(ctx);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any ExpressionVisitor::visitAddSubtract(ExpressionParser::AddSubtractContext *ctx)
{
    auto node = std::make_shared<ExpressionBinaryOperatorNode>(ctx->Add() ? ExpressionBinaryOperatorNode::OperatorAdd
                                                                          : ExpressionBinaryOperatorNode::OperatorSubtract);

    currentContext()->addChild(node);

    contextStack_.push_back(node);

    auto result = visitChildren(ctx);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any ExpressionVisitor::visitFunction(ExpressionParser::FunctionContext *ctx)
{
    // Check that the function name is valid
    if (!ExpressionFunctionNode::internalFunctions().isValid(ctx->Name()->getText()))
        throw(ExpressionExceptions::ExpressionSyntaxException(
            fmt::format("'{}' is not a valid internal function.", ctx->Name()->getText())));
    ExpressionFunctionNode::InternalFunction func =
        ExpressionFunctionNode::internalFunctions().enumeration(ctx->Name()->getText());

    auto node = std::make_shared<ExpressionFunctionNode>(func);

    currentContext()->addChild(node);

    contextStack_.push_back(node);

    auto result = visitChildren(ctx);

    contextStack_.pop_back();

    // Check number of args that were given...
    if (!ExpressionFunctionNode::internalFunctions().validNArgs(func, node->nChildren()))
        throw(ExpressionExceptions::ExpressionSyntaxException(
            fmt::format("Internal function '{}' was given the wrong number of arguments.", ctx->Name()->getText())));

    return result;
}

antlrcpp::Any ExpressionVisitor::visitVariable(ExpressionParser::VariableContext *ctx)
{
    // Check local variables first
    const std::vector<std::shared_ptr<ExpressionVariable>> &localVars = *localVariables_;
    auto it = std::find_if(localVars.begin(), localVars.end(),
                           [ctx](auto var) { return DissolveSys::sameString(var->baseName(), ctx->Name()->getText()); });
    if (it == localVars.end())
    {
        // Do we have any external variables available?
        if (!externalVariables_)
            throw(ExpressionExceptions::ExpressionSemanticException(fmt::format(
                "Variable '{}' does not exist in this context (there are no variables defined).\n", ctx->Name()->getText())));

        // Does the named variable exist?
        const std::vector<std::shared_ptr<ExpressionVariable>> &extVars = *externalVariables_;
        it = std::find_if(extVars.begin(), extVars.end(),
                          [ctx](auto var) { return DissolveSys::sameString(var->name(), ctx->Name()->getText()); });
        if (it == extVars.end())
            throw(ExpressionExceptions::ExpressionSemanticException(
                fmt::format("Variable '{}' does not exist in this context.\n", ctx->Name()->getText())));
    }

    // Create the new node pointing to the variable
    auto node = std::make_shared<ExpressionReferenceNode>(*it);

    currentContext()->addChild(node);

    return visitChildren(ctx);
}

antlrcpp::Any ExpressionVisitor::visitValue(ExpressionParser::ValueContext *ctx)
{
    if (ctx->Integer())
        currentContext()->addChild(std::make_shared<ExpressionNumberNode>(std::stoi(ctx->Integer()->getText())));
    else if (ctx->Double())
        currentContext()->addChild(std::make_shared<ExpressionNumberNode>(std::stod(ctx->Double()->getText())));

    return visitChildren(ctx);
}
