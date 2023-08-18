// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "expression/expression.h"
#include "ExpressionLexer.h"
#include "ExpressionParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "expression/ExpressionErrorListeners.h"
#include "expression/ExpressionVisitor.h"
#include "expression/root.h"
#include "expression/variable.h"
#include <algorithm>

Expression::Expression(std::string_view expressionText) : rootNode_(nullptr) { create(expressionText); }

Expression::~Expression() { clearNodes(); }

Expression::Expression(const Expression &source) { (*this) = source; }

void Expression::operator=(const Expression &source)
{
    clearNodes();

    if (source.rootNode_)
        rootNode_ = source.rootNode_->duplicate();
}

/*
 * Data
 */

// Add local variable
std::shared_ptr<ExpressionVariable> Expression::addLocalVariable(std::string_view name)
{
    if (std::find_if(localVariables_.begin(), localVariables_.end(),
                     [name](const auto &var) { return DissolveSys::sameString(name, var->name()); }) != localVariables_.end())
        throw(std::runtime_error(
            fmt::format("Tried to create local variable '{}' in Expression, but it already exists.\n", name)));

    return localVariables_.emplace_back(std::make_shared<ExpressionVariable>(name));
}

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

    // If the string is empty, can return now
    if (expressionString.empty())
        return true;

    rootNode_ = std::make_shared<ExpressionRootNode>();

    // Create string stream and set up ANTLR input stream
    std::stringstream stream;
    stream << expressionString;
    antlr4::ANTLRInputStream input(stream);

    // Create ANTLR lexer and set-up error listener
    ExpressionLexer lexer(&input);
    ExpressionLexerErrorListener lexerErrorListener(expressionString);
    lexer.removeErrorListeners();
    lexer.addErrorListener(&lexerErrorListener);

    // Generate tokens from input stream
    antlr4::CommonTokenStream tokens(&lexer);

    // Create ANTLR parser and set-up error listeners
    ExpressionParser parser(&tokens);
    ExpressionParserErrorListener parserErrorListener(expressionString);
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
        visitor.create(*this, tree, localVariables_, externalVariables);
    }
    catch (std::exception &ex)
    {
        fmt::print(ex.what());
        clearNodes();
        return Messenger::error(ex.what());
    }

    return true;
}

// Return expression string
std::string Expression::expressionString() const { return rootNode_ ? rootNode_->asString() : ""; }

// Return root node for the expression
std::shared_ptr<ExpressionNode> Expression::rootNode() { return rootNode_; }

/*
 * Execution
 */

// Execute expression
std::optional<ExpressionValue> Expression::evaluate() const
{
    // If there is no rootNode_ (i.e. expression) this is valid, and we just return an empty value
    if (!rootNode_)
        return ExpressionValue();

    return rootNode_->evaluate();
}

// Execute and return as integer
int Expression::asInteger() const
{
    auto result = evaluate();
    if (!result)
        throw(std::runtime_error(fmt::format("Failed to evaluate expression '{}'.", expressionString())));

    return (*result).asInteger();
}

// Execute and return as double
double Expression::asDouble() const
{
    auto result = evaluate();
    if (!result)
        throw(std::runtime_error(fmt::format("Failed to evaluate expression '{}'.", expressionString())));

    return (*result).asDouble();
}
