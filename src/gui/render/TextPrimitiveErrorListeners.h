// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <antlr4-runtime.h>
#include <exception>

namespace TextPrimitiveExceptions
{
// TextPrimitive Syntax Exception
class TextPrimitiveSyntaxException : public std::exception
{
    public:
    TextPrimitiveSyntaxException(std::string_view message = "Undefined TextPrimitive Syntax Exception") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    virtual const char *what() const throw() { return message_.c_str(); }
};

// TextPrimitive Internal Error Exception
class TextPrimitiveInternalErrorException : public std::exception
{
    public:
    TextPrimitiveInternalErrorException(std::string_view message = "TextPrimitive internal error.") : message_{message} {}

    private:
    // Input string to TextPrimitive
    std::string message_;

    public:
    virtual const char *what() const throw() { return message_.c_str(); }
};
} // namespace TextPrimitiveExceptions

// TextPrimitive Lexer Error Listener
class TextPrimitiveLexerErrorListener : public antlr4::BaseErrorListener
{
    public:
    TextPrimitiveLexerErrorListener(const std::string inputString);

    private:
    // Input string to TextPrimitive
    const std::string inputString_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};

// TextPrimitive Parser Error Listener
class TextPrimitiveParserErrorListener : public antlr4::BaseErrorListener
{
    public:
    TextPrimitiveParserErrorListener(const std::string inputString);

    private:
    // Input string to TextPrimitive
    const std::string inputString_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};
