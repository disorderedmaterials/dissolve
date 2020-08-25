/*
    *** Mathematical Expression Generator
    *** src/expression/generator.h
    Copyright T. Youngs 2015-2020

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

#pragma once

#include "base/enumoptions.h"
#include "templates/reflist.h"

// External declarations
extern int ExpressionGenerator_parse();

// Forward declarations
class Expression;
class ExpressionVariable;

// Expression Generator
class ExpressionGenerator
{
    private:
    ExpressionGenerator(Expression &expression, std::string_view expressionText);

    public:
    ~ExpressionGenerator();

    /*
     * Target Expression and Generator
     */
    private:
    // Pointer to target Expression
    static Expression *expression_;
    // The ExpressionGenerator object calling the parser/lexer
    static ExpressionGenerator *generator_;

    public:
    // Return target Expression (static to allow ExpressionGenerator_parse() to use it)
    static Expression *expression();
    // Return current ExpressionGenerator (static to allow ExpressionGenerator_parse() to use it)
    static ExpressionGenerator *generator();

    /*
     * Lexer
     */
    public:
    // Symbol Tokens
    enum SymbolToken
    {
        AssignSymbol,
        GEQSymbol,
        LEQSymbol,
        CNEQSymbol,
        FNEQSymbol,
        AndSymbol,
        OrSymbol,
        nSymbolTokens
    };
    // Return enum options for SymbolToken
    static EnumOptions<int> symbolTokens();

    private:
    // Source expression string
    std::string expressionString_;
    // Integer position in stringSource, total length of string, and starting position of current token/function
    int stringPos_, stringLength_, tokenStart_, functionStart_;
    // Whether to use additional pre-defined constants
    bool useAdditionalConstants_;

    private:
    // Set string source for lexer
    void setSource(std::string_view expressionText);
    // Get next character from current input stream
    char getChar();
    // Peek next character from current input stream
    char peekChar();
    // 'Replace' last character read from current input stream
    void unGetChar();

    public:
    // Parser lexer, called by yylex()
    int lex();

    /*
     * Creation
     */
    public:
    // Static generation functions
    static bool generate(Expression &expression);
    static bool generate(Expression &expression, RefList<ExpressionVariable> externalVariables);
};
