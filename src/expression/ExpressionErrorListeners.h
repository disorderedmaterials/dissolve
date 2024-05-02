// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    explicit ExpressionSyntaxException(std::string_view message = "Undefined Expression Syntax Exception") : message_{message}
    {
    }

    private:
    // Error message
    std::string message_;

    public:
    const char *what() const noexcept override { return message_.c_str(); }
};

// Expression Semantic Exception
class ExpressionSemanticException : public std::exception
{
    public:
    explicit ExpressionSemanticException(std::string_view message = "Undefined Expression Semantic Exception")
        : message_{message}
    {
    }

    private:
    // Error message
    std::string message_;

    public:
    const char *what() const noexcept override { return message_.c_str(); }
};

// Expression Internal Error Exception
class ExpressionInternalErrorException : public std::exception
{
    public:
    explicit ExpressionInternalErrorException(std::string_view message = "Expression internal error.") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    const char *what() const noexcept override { return message_.c_str(); }
};
} // namespace ExpressionExceptions

// Expression Lexer Error Listener
class ExpressionLexerErrorListener : public antlr4::BaseErrorListener
{
    public:
    ExpressionLexerErrorListener(std::string_view inputLine);

    private:
    // Input line for expression generation
    std::string_view inputLine_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep) override;
};

// Expression Parser Error Listener
class ExpressionParserErrorListener : public antlr4::BaseErrorListener
{
    public:
    ExpressionParserErrorListener(std::string_view inputLine);

    private:
    // Input line for expression generation
    std::string_view inputLine_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep) override;
};
