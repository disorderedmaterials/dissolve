// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/TextPrimitiveErrorListeners.h"
#include "base/messenger.h"
#include "base/sysFunc.h"

/*
 * TextPrimitive Lexer Error Listener
 */

TextPrimitiveLexerErrorListener::TextPrimitiveLexerErrorListener(const std::string inputString) : inputString_{inputString} {}

void TextPrimitiveLexerErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                                  size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in TextPrimitive definition '{}'\n", inputString_);
    std::string marker(34 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw TextPrimitiveExceptions::TextPrimitiveSyntaxException(fmt::format("Syntax Error: {}", escaped));
}

/*
 * TextPrimitive Parser Error Listener
 */

TextPrimitiveParserErrorListener::TextPrimitiveParserErrorListener(const std::string inputString) : inputString_{inputString} {}

void TextPrimitiveParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                                   size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in TextPrimitive definition '{}'\n", inputString_);
    std::string marker(34 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw TextPrimitiveExceptions::TextPrimitiveSyntaxException(fmt::format("Syntax Error: {}", escaped));
}
