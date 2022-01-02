// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "expression/expression.h"
#include "ExpressionLexer.h"
#include "ExpressionParser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "expression/ExpressionErrorListeners.h"
#include "expression/ExpressionVisitor.h"
#include "expression/root.h"
#include <algorithm>
#include <cstdarg>
#include <cstring>

Expression::Expression(std::string_view expressionText) : rootNode_(nullptr) { create(expressionText); }

Expression::~Expression() { clearNodes(); }

Expression::Expression(const Expression &source) { (*this) = source; }

void Expression::operator=(const Expression &source)
{
    clearNodes();

    expressionString_ = source.expressionString_;

    if (source.rootNode_)
        rootNode_ = source.rootNode_->duplicate();
}

/*
 * Data
 */

// Clear node data
void Expression::clearNodes()
{
    if (rootNode_)
        rootNode_->clear();

    rootNode_ = nullptr;
}

// Return whether current expression is valid
bool Expression::isValid() const { return rootNode_ != nullptr; }

// Create expression from supplied string, with optional external variables
bool Expression::create(std::string_view expressionString,
                        OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> externalVariables)
{
    clearNodes();

    expressionString_ = expressionString;

    // If the string is empty, can return now
    if (expressionString_.empty())
        return true;

    rootNode_ = std::make_shared<ExpressionRootNode>();

    // Create string stream and set up ANTLR input stream
    std::stringstream stream;
    stream << expressionString_;
    antlr4::ANTLRInputStream input(stream);

    // Create ANTLR lexer and set-up error listener
    ExpressionLexer lexer(&input);
    ExpressionLexerErrorListener lexerErrorListener(*this);
    lexer.removeErrorListeners();
    lexer.addErrorListener(&lexerErrorListener);

    // Generate tokens from input stream
    antlr4::CommonTokenStream tokens(&lexer);

    // Create ANTLR parser and set-up error listeners
    ExpressionParser parser(&tokens);
    ExpressionParserErrorListener parserErrorListener(*this);
    parser.removeErrorListeners();
    parser.removeParseListeners();
    parser.addErrorListener(&lexerErrorListener);
    parser.addErrorListener(&parserErrorListener);

    // Generate the AST
    ExpressionParser::ExpressionContext *tree = nullptr;
    try
    {
        tree = parser.expression();
    }
    catch (ExpressionExceptions::ExpressionSyntaxException &ex)
    {
        fmt::print(ex.what());
        clearNodes();
        return Messenger::error(ex.what());
    };

    // Visit the nodes in the AST
    ExpressionVisitor visitor;
    try
    {
        visitor.create(*this, tree, externalVariables);
    }
    catch (ExpressionExceptions::ExpressionSyntaxException &ex)
    {
        fmt::print(ex.what());
        clearNodes();
        return Messenger::error(ex.what());
    }

    return true;
}

// Return original generating string
std::string_view Expression::expressionString() const { return expressionString_; }

// Set generating string from current nodes
void Expression::setExpressionStringFromNodes() { expressionString_ = rootNode_ ? rootNode_->asString() : ""; }

// Return root node for the expression
std::shared_ptr<ExpressionNode> Expression::rootNode() { return rootNode_; }

/*
 * Execution
 */

// Execute expression
std::optional<ExpressionValue> Expression::evaluate() const
{
    if (rootNode_)
        return rootNode_->evaluate();

    return std::nullopt;
}

// Execute and return as integer
int Expression::asInteger() const
{
    auto result = evaluate();
    if (!result)
        throw(std::runtime_error(fmt::format("Failed to evaluate expression '{}'.", expressionString_)));

    return (*result).asInteger();
}

// Execute and return as double
double Expression::asDouble() const
{
    auto result = evaluate();
    if (!result)
        throw(std::runtime_error(fmt::format("Failed to evaluate expression '{}'.", expressionString_)));

    return (*result).asDouble();
}
