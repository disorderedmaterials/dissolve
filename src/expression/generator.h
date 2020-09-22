// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
