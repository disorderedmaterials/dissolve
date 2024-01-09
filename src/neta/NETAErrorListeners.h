// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <antlr4-runtime.h>
#include <exception>

// Forward Declarations
class NETADefinition;

namespace NETAExceptions
{
// NETA Syntax Exception
class NETASyntaxException : public std::exception
{
    public:
    NETASyntaxException(std::string_view message = "Undefined NETA Syntax Exception") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    const char *what() const noexcept override { return message_.c_str(); }
};

// NETA Internal Error Exception
class NETAInternalErrorException : public std::exception
{
    public:
    NETAInternalErrorException(std::string_view message = "NETA internal error.") : message_{message} {}

    private:
    // Error message
    std::string message_;

    public:
    const char *what() const noexcept override { return message_.c_str(); }
};
} // namespace NETAExceptions

// NETA Lexer Error Listener
class NETALexerErrorListener : public antlr4::BaseErrorListener
{
    public:
    NETALexerErrorListener(const NETADefinition &currentDefinition);

    private:
    // NETA definition being constructed
    const NETADefinition &neta_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep) override;
};

// NETA Parser Error Listener
class NETAParserErrorListener : public antlr4::BaseErrorListener
{
    public:
    NETAParserErrorListener(const NETADefinition &currentDefinition);

    private:
    // NETA definition being constructed
    const NETADefinition &neta_;

    /*
     * BaseErrorListener Overrides
     */
    public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *, size_t line, size_t charPositionInLine,
                     const std::string &, std::exception_ptr ep) override;
};
