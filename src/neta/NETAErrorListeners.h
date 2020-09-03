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

#ifndef DISSOLVE_NETA_ERRORLISTENERS_H
#define DISSOLVE_NETA_ERRORLISTENERS_H

#include <antlr4-runtime.h>
#include <exception>

// Forward Declarations
class NETADefinition;

namespace NETAExceptions
{

// NETA Syntax Exception
class NETASyntaxException : public std::exception
{
    virtual const char *what() const throw() { return "NETA Syntax Exception"; }
};
static NETASyntaxException syntaxException;
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
                     const std::string &, std::exception_ptr ep);
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
                     const std::string &, std::exception_ptr ep);
};

#endif
