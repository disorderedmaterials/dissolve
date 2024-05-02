// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
bool isElement(std::string symbol) { return Elements::element(symbol) != Elements::Unknown; }
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
fragment SYMBOL: [_+-];

// Comparison Operators
SizeOperator: '<=' | '>=' | '<' | '>';
EqualityOperator: '=' | '!=';

// Punctuation
OpenParenthesis: '(';
CloseParenthesis: ')';
OpenSquareBracket: '[';
CloseSquareBracket: ']';
Comma: ',';
TypeReference: '&';

// Whitespace
WS : [ ]+ -> skip;

// Basic Tokens
Integer: DIGIT+;

// Keywords
Or: '|';
Not: '!' | '¬';
BondCountKeyword: 'n' 'b' 'o' 'n' 'd' 's';
CharacterKeyword: '?';
ConnectionKeyword: '-';
GeometryKeyword: 'g' 'e' 'o' 'm' 'e' 't' 'r' 'y';
HydrogenCountKeyword: 'n' 'h';
IdKeyword: '#';
RingKeyword: 'r' 'i' 'n' 'g';

// Named Tokens
Element: UCASELETTER LCASELETTER*;
FFTypeName: TypeReference LETTER (LETTER | DIGIT | SYMBOL)*;
FFTypeIndex: TypeReference Integer;
Text: LCASELETTER+;
