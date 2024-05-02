// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <antlr4-runtime.h>
#include <exception>

namespace CIFImportExceptions
{
// CIFImport Syntax Exception
class CIFImportSyntaxException : public std::exception
{
    public:
    CIFImportSyntaxException(std::string_view message = "Undefined CIFImport Syntax Exception") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    virtual const char *what() const throw() { return message_.c_str(); }
};
} // namespace CIFImportExceptions

// CIFImport Lexer Error Listener
class CIFImportLexerErrorListener : public antlr4::BaseErrorListener
{
    public:
    CIFImportLexerErrorListener();

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};

// CIFImport Parser Error Listener
class CIFImportParserErrorListener : public antlr4::BaseErrorListener
{
    public:
    CIFImportParserErrorListener();

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep);
};
