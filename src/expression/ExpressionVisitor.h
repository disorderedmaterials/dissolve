// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "ExpressionParserBaseVisitor.h"
#include "expression/expression.h"
#include "templates/optionalRef.h"
#include <antlr4-runtime.h>

// Expression Visitor for ANTLR
class ExpressionVisitor : ExpressionParserBaseVisitor
{
    /*
     * Creation Entry-Point
     */
    private:
    // Target Expression
    Expression *expression_;
    // Variables available to this expression
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> localVariables_;
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> externalVariables_;
    // Context stack
    std::vector<std::shared_ptr<ExpressionNode>> contextStack_;

    private:
    // Return the topmost context in the stack
    std::shared_ptr<ExpressionNode> currentContext() const;

    public:
    // Construct description within supplied object, from given tree
    void create(Expression &expr, ExpressionParser::ExpressionContext *tree,
                const std::vector<std::shared_ptr<ExpressionVariable>> &localVariables,
                OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> externalVariables);

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
    antlrcpp::Any visitComparison(ExpressionParser::ComparisonContext *ctx) override;
    antlrcpp::Any visitFunction(ExpressionParser::FunctionContext *ctx) override;
    antlrcpp::Any visitVariable(ExpressionParser::VariableContext *ctx) override;
    antlrcpp::Any visitValue(ExpressionParser::ValueContext *ctx) override;
};
