// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/ExpressionErrorListeners.h"
#include "base/messenger.h"
#include "expression/expression.h"

/*
 * Expression Lexer Error Listener
 */

ExpressionLexerErrorListener::ExpressionLexerErrorListener(std::string_view inputLine) : inputLine_(inputLine) {}

void ExpressionLexerErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                               size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in Expression definition '{}'\n", inputLine_);
    std::string marker(32 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw ExpressionExceptions::ExpressionSyntaxException(fmt::format("Syntax Error: {}", escaped));
}

/*
 * Expression Parser Error Listener
 */

ExpressionParserErrorListener::ExpressionParserErrorListener(std::string_view inputLine) : inputLine_(inputLine) {}

void ExpressionParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                                size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in Expression definition '{}'\n", inputLine_);
    std::string marker(32 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw ExpressionExceptions::ExpressionSyntaxException(fmt::format("Syntax Error: {}", escaped));
}
