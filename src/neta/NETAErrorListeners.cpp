// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/NETAErrorListeners.h"
#include "base/messenger.h"
#include "neta/neta.h"

/*
 * NETA Lexer Error Listener
 */

NETALexerErrorListener::NETALexerErrorListener(const NETADefinition &currentDefinition) : neta_(currentDefinition) {}

void NETALexerErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                         size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in NETA definition '{}'\n", neta_.definitionString());
    std::string marker(26 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker);

    throw NETAExceptions::NETASyntaxException(fmt::format("Bad syntax: {}", DissolveSys::doubleChars(message, "{}")));
}

/*
 * NETA Parser Error Listener
 */

NETAParserErrorListener::NETAParserErrorListener(const NETADefinition &currentDefinition) : neta_(currentDefinition) {}

void NETAParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                          size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in NETA definition '{}'\n", neta_.definitionString());
    std::string marker(26 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker);

    throw NETAExceptions::NETASyntaxException(fmt::format("Parse error: {}", DissolveSys::doubleChars(message, "{}")));
}
