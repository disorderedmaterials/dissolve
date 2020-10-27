// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

lexer grammar NETALexer;

// Lexer file header
@lexer::header {/* NETA ANTLR Lexer */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@lexer::postinclude {
#include "data/elements.h"
}

// Directly precedes the lexer class declaration in the h file (e.g. for additional types etc.).
@lexer::context {/* lexer context section */}

// Appears in the private part of the lexer in the h file.
@lexer::declarations {
bool isElement(std::string symbol) { return Elements::element(symbol.c_str()).Z() != 0; }
}

// Appears in line with the other class member definitions in the cpp file.
@lexer::definitions {/* lexer definitions section */}

/*
 * Lexer Rules
 */

// Fragments
fragment DIGIT: [0-9];
fragment LETTER: [a-zA-Z];
fragment UCASELETTER: [A-Z];
fragment LCASELETTER: [a-z];

// Comparison Operators
ComparisonOperator: '<=' | '>=' | '<' | '>' | '=' | '!=';

// Punctuation
OpenParenthesis: '(';
CloseParenthesis: ')';
OpenSquareBracket: '[';
CloseSquareBracket: ']';
Comma: ',';
TypeReference: '&';

// Whitespace
WS : [ ]* -> skip;

// Basic Tokens
Integer: DIGIT+;

// Keywords
ConnectionKeyword: '-';
RingKeyword: 'r' 'i' 'n' 'g';

// Named Tokens
Element: UCASELETTER LCASELETTER* { isElement(getText()) }?;
FFTypeName: TypeReference LETTER+;
FFTypeIndex: TypeReference Integer;
Keyword: LCASELETTER+;
