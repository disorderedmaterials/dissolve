// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "ExpressionParserBaseVisitor.h"
#include "expression/expressionNEW.h"
#include <antlr4-runtime.h>

// Expression Visitor for ANTLR
class ExpressionVisitor : ExpressionParserBaseVisitor
{
    /*
     * Creation Entry-Point
     */
    private:
    // Target ExpressionDefinition
    ExpressionNEW *expression_;
    // External variables available to this expression
    RefList<ExpressionVariable> externalVariables_;
    // Context stack
    std::vector<std::shared_ptr<ExpressionNodeNEW>> contextStack_;

    private:
    // Return the topmost context in the stack
    std::shared_ptr<ExpressionNodeNEW> currentContext() const;

    public:
    // Construct description within supplied object, from given tree
    void create(ExpressionNEW &expr, ExpressionParser::ExpressionContext *tree, RefList<ExpressionVariable> externalVariables);

    /*
     * Visitor Overrides
     */
    private:
    // Nodes
    antlrcpp::Any visitParentheses(ExpressionParser::ParenthesesContext *ctx) override;
    antlrcpp::Any visitUnaryMinus(ExpressionParser::UnaryMinusContext *ctx) override;
    antlrcpp::Any visitPower(ExpressionParser::PowerContext *ctx) override;
    antlrcpp::Any visitMultiplyDivide(ExpressionParser::MultiplyDivideContext *ctx) override;
    antlrcpp::Any visitAddSubtract(ExpressionParser::AddSubtractContext *ctx) override;
    antlrcpp::Any visitFunction(ExpressionParser::FunctionContext *ctx) override;
    antlrcpp::Any visitVariable(ExpressionParser::VariableContext *ctx) override;
    antlrcpp::Any visitValue(ExpressionParser::ValueContext *ctx) override;
};
