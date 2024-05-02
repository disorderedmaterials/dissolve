// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/CIFImportErrorListeners.h"
#include "base/messenger.h"
#include "base/sysFunc.h"

/*
 * CIFImport Lexer Error Listener
 */

CIFImportLexerErrorListener::CIFImportLexerErrorListener() {}

void CIFImportLexerErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                              size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in CIFImport definition on line {}, column {}.\n", line, charPositionInLine);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw CIFImportExceptions::CIFImportSyntaxException(fmt::format("Syntax Error: {}", escaped));
}

/*
 * CIFImport Parser Error Listener
 */

CIFImportParserErrorListener::CIFImportParserErrorListener() {}

void CIFImportParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                               size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("\nError in CIFImport definition on line {}, column {}.\n", line, charPositionInLine);

    // The actual error message can contain braces, so escape those to avoid breaking fmt
    auto escaped = DissolveSys::replace(DissolveSys::replace(message, "{", "{{"), "}", "}}");

    throw CIFImportExceptions::CIFImportSyntaxException(fmt::format("Syntax Error: {}", escaped));
}
