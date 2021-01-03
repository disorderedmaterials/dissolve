// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <antlr4-runtime.h>
#include <exception>

// Forward Declarations
class Expression;

namespace ExpressionExceptions
{
// Expression Syntax Exception
class ExpressionSyntaxException : public std::exception
{
    public:
    ExpressionSyntaxException(std::string_view message = "Undefined Expression Syntax Exception") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    virtual const char *what() const throw() { return message_.c_str(); }
};

// Expression Internal Error Exception
class ExpressionInternalErrorException : public std::exception
{
    public:
    ExpressionInternalErrorException(std::string_view message = "Expression internal error.") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    virtual const char *what() const throw() { return message_.c_str(); }
};
} // namespace ExpressionExceptions

// Expression Lexer Error Listener
class ExpressionLexerErrorListener : public antlr4::BaseErrorListener
{
    public:
    ExpressionLexerErrorListener(const Expression &expr);

    private:
    // Expression being constructed
    const Expression &expression_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};

// Expression Parser Error Listener
class ExpressionParserErrorListener : public antlr4::BaseErrorListener
{
    public:
    ExpressionParserErrorListener(const Expression &expr);

    private:
    // Expression being constructed
    const Expression &expression_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};
