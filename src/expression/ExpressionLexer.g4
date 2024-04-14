// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

lexer grammar ExpressionLexer;

// Lexer file header
@lexer::header {/* Expression ANTLR Lexer */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@lexer::postinclude { }

// Directly precedes the lexer class declaration in the h file (e.g. for additional types etc.).
@lexer::context {/* lexer context section */}

// Appears in the private part of the lexer in the h file.
@lexer::declarations { }

// Appears in line with the other class member definitions in the cpp file.
@lexer::definitions {/* lexer definitions section */}

/*
 * Lexer Rules
 */

// Fragments
fragment DIGIT: [0-9];
fragment LETTER: [a-zA-Z];
fragment EXPONENT: ('e'|'E') ('+'|'-')? ('0'..'9')+;
 
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

// Boolean Operators
LessEqual: '<=';
GreaterEqual: '>=';
Equal: '==';
LessThan: '<';
GreaterThan: '>';

// Named Token
Name: LETTER (LETTER | DIGIT | '.')*;
