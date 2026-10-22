// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <antlr4-runtime.h>
#include <exception>

namespace CIFExceptions
{
// CIF Syntax Exception
class CIFSyntaxException : public std::exception
{
    public:
    CIFSyntaxException(std::string_view message = "Undefined CIF Syntax Exception") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    virtual const char *what() const throw() { return message_.c_str(); }
};
} // namespace CIFExceptions

// CIF Lexer Error Listener
class CIFLexerErrorListener : public antlr4::BaseErrorListener
{
    public:
    CIFLexerErrorListener();

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};

// CIF Parser Error Listener
class CIFParserErrorListener : public antlr4::BaseErrorListener
{
    public:
    CIFParserErrorListener();

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};
