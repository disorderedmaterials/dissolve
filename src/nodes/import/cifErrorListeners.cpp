// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/import/cifErrorListeners.h"
#include "base/messenger.h"
#include "base/sysFunc.h"

/*
 * CIF Lexer Error Listener
 */

CIFLexerErrorListener::CIFLexerErrorListener() {}

void CIFLexerErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                        size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in CIF definition on line {}, column {}.\n", line, charPositionInLine);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw CIFExceptions::CIFSyntaxException(std::format("Syntax Error: {}", escaped));
}

/*
 * CIF Parser Error Listener
 */

CIFParserErrorListener::CIFParserErrorListener() {}

void CIFParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                         size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in CIF definition on line {}, column {}.\n", line, charPositionInLine);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw CIFExceptions::CIFSyntaxException(std::format("Syntax Error: {}", escaped));
}
