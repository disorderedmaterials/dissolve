// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/ExpressionVisitor.h"
#include "data/ff.h"
#include "expression/ExpressionErrorListeners.h"
#include "expression/binary.h"
#include "expression/function.h"
#include "expression/number.h"
#include "expression/reference.h"
#include "expression/unary.h"
#include "expression/variable.h"
#include "templates/optionalref.h"

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
void ExpressionVisitor::create(Expression &expr, ExpressionParser::ExpressionContext *tree,
                               RefList<ExpressionVariable> externalVariables)
{
    expression_ = &expr;
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
    auto node = std::make_shared<ExpressionRootNode>();

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
    const auto nArgs = ExpressionFunctionNode::internalFunctions().minArgs(func);
    if (node->nChildren() != nArgs)
        throw(ExpressionExceptions::ExpressionSyntaxException(
            fmt::format("Internal function '{}' expects exactly {} {} but {} {} given.", ctx->Name()->getText(), nArgs,
                        nArgs == 1 ? "argument" : "arguments", node->nChildren(), node->nChildren() == 1 ? "was" : "were")));

    return result;
}

antlrcpp::Any ExpressionVisitor::visitVariable(ExpressionParser::VariableContext *ctx)
{
    // Does the named variable exist?
    auto it = std::find_if(externalVariables_.begin(), externalVariables_.end(),
                           [ctx](auto var) { return DissolveSys::sameString(var->name(), ctx->Name()->getText()); });
    if (it == externalVariables_.end())
        throw(ExpressionExceptions::ExpressionSyntaxException(
            fmt::format("Variable '{}' does not exist in this context.", ctx->Name()->getText())));

    auto node = std::make_shared<ExpressionReferenceNode>(it.item());

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

/*
antlrcpp::Any ExpressionVisitor::visitFlag(ExpressionParser::FlagContext *context)
{
    if (currentExpressionContext()->isValidFlag(context->Keyword()->getText().c_str()))
    {
        if (!currentExpressionContext()->setFlag(context->Keyword()->getText().c_str(), true))
            throw(ExpressionExceptions::ExpressionSyntaxException(
                fmt::format("Failed to set flag '{}' for the current context ({}).", context->Keyword()->getText().c_str(),
                            ExpressionNode::nodeTypes().keyword(currentExpressionContext()->nodeType()))));
    }
    else
        throw(ExpressionExceptions::ExpressionSyntaxException(
            fmt::format("'{}' is not a valid flag for the current context ({}).", context->Keyword()->getText().c_str(),
                        ExpressionNode::nodeTypes().keyword(currentExpressionContext()->nodeType()))));

    return visitChildren(context);
}*/
