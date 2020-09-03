/*
    *** NETA Error Listeners
    *** src/neta/NETAErrorListeners.h
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    Messenger::print("Error in NETA definition '{}'\n", neta_.definitionString());
    std::string marker(26 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker.c_str());
    Messenger::print("Reason: {}\n", message.c_str());

    throw NETAExceptions::syntaxException;
}

/*
 * NETA Parser Error Listener
 */

NETAParserErrorListener::NETAParserErrorListener(const NETADefinition &currentDefinition) : neta_(currentDefinition) {}

void NETAParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *token, size_t line,
                                          size_t charPositionInLine, const std::string &message, std::exception_ptr ep)
{
    Messenger::print("Error in NETA definition '{}'\n", neta_.definitionString());
    std::string marker(26 + charPositionInLine, ' ');
    marker += '^';
    Messenger::print("{}\n", marker.c_str());
    Messenger::print("Reason: {}\n", message.c_str());

    throw NETAExceptions::syntaxException;
}
