// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

lexer grammar ExpressionLexer;

// Lexer file header
@lexer::header {/* Expression ANTLR Lexer */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@lexer::postinclude { }

// Directly precedes the lexer class declaration in the h file (e.g. for additional types etc.).
@lexer::context {/* lexer context section */}

// Appears in the private part of the lexer in the h file.
@lexer::declarations {
bool isVariable(std::string symbol) { return false; /* Elements::element(symbol.c_str()).Z() != 0; */ }
}

// Appears in line with the other class member definitions in the cpp file.
@lexer::definitions {/* lexer definitions section */}

/*
 * Lexer Rules
 */

// Fragments
fragment DIGIT: [0-9];
fragment LETTER: [a-zA-Z];
fragment EXPONENT: ('e'|'E') ('+'|'-')? ('0'..'9')+;
 
// Comparison Operators
ComparisonOperator: '<=' | '>=' | '<' | '>' | '=' | '!=';

// Punctuation
OpenParenthesis: '(';
CloseParenthesis: ')';
Comma: ',';

// Whitespace
WS : [ ]+ -> skip;

// Numerical Tokens
Integer: DIGIT+ EXPONENT?;
Double: DIGIT* '.' DIGIT+ EXPONENT?;

// Operators
Multiply: '*';
Divide: '/';
Add: '+';
Subtract: '-';
Power: '^';

// Named Token
Name: LETTER+;
// Variable: LETTER+ { isVariable(getText()) }?;
