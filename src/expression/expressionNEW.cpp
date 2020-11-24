// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/expressionNEW.h"
#include "ExpressionLexer.h"
#include "ExpressionParser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "expression/ExpressionErrorListeners.h"
#include "expression/ExpressionVisitor.h"
#include "expression/root.h"
#include <algorithm>
#include <stdarg.h>
#include <string.h>

ExpressionNEW::ExpressionNEW(std::string_view expressionText)
{
    if (!expressionText.empty())
        create(expressionText);
}

ExpressionNEW::~ExpressionNEW() { clear(); }

ExpressionNEW::ExpressionNEW(const ExpressionNEW &source) { (*this) = source; }

void ExpressionNEW::operator=(const ExpressionNEW &source)
{
    throw(std::runtime_error("TODO Expression::operator= not implemented"));
    // Reset our structure, and regenerate from the expression string
    //     clear();

    //     expressionString_ = source.expressionString_;

    //     ExpressionNEWGenerator::generate(*this, source.externalVariables_);
}

/*
 * Data
 */

// Clear data
void ExpressionNEW::clear()
{
    expressionString_ = "";

    if (rootNode_)
        rootNode_->clear();

    rootNode_ = nullptr;
}

// Return whether current expression is valid
bool ExpressionNEW::isValid() const { return rootNode_ != nullptr; }

// Create expression from supplied string, with optional external variables
bool ExpressionNEW::create(std::string_view expressionString, RefList<ExpressionVariable> externalVariables)
{
    clear();

    expressionString_ = expressionString;
    rootNode_ = std::make_shared<ExpressionRootNode>();

    // Create string stream and set up ANTLR input strem
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

    // Create ANTLR parser and set-up error listenres
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
        return Messenger::error(ex.what());
    }

    return true;
}

// Return original generating string
std::string_view ExpressionNEW::expressionString() const { return expressionString_; }

// Return root node for the expression
std::shared_ptr<ExpressionRootNode> ExpressionNEW::rootNode() { return rootNode_; }

/*
 * Execution
 */

// Execute expression
std::optional<ExpressionValue> ExpressionNEW::evaluate()
{
    if (rootNode_)
        return rootNode_->evaluate();

    return std::nullopt;
}

// Execute and return as integer
int ExpressionNEW::asInteger()
{
    auto result = evaluate();
    if (!result)
        throw(std::runtime_error(fmt::format("Failed to evaluate expression '{}'.", expressionString_)));

    return (*result).asInteger();
}

// Execute and return as double
double ExpressionNEW::asDouble()
{
    auto result = evaluate();
    if (!result)
        throw(std::runtime_error(fmt::format("Failed to evaluate expression '{}'.", expressionString_)));

    return (*result).asDouble();
}
