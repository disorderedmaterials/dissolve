// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

lexer grammar CIFImportLexer;

// Lexer file header
@lexer::header {/* CIFImport ANTLR Lexer */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@lexer::postinclude {
    #include "base/sysFunc.h"
    #include "io/import/CIFImportVisitor.h"
}

// Directly precedes the lexer class declaration in the h file (e.g. for additional types etc.).
@lexer::context {/* lexer context section */}

// Appears in the private part of the lexer in the h file.
@lexer::declarations { }

// Appears in line with the other class member definitions in the cpp file.
@lexer::definitions {/* lexer definitions section */}

/*
 * Lexer Rules
 *
 * Rules defined here are based on those specified for CIF 1.1 (https://www.iucr.org/resources/cif/spec/version1.1/cifsyntax#bnf).
 * They are reproduced as faithfully as possible, with small modifications made along the way (e.g. EOL tokens are skipped rather than matched as part of tokens for the most part.
 * Quotes and bracketed errors are stripped from the generated tokens.
 */

// Fragments
fragment OrdinaryChar:	'!' | '%' | '&' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | '<' | '=' | '>' | '?' | '@' | [A-Z] | '\\' | '^' | '`' | [a-z] | '{' | '|' | '}' | '~';
fragment NonBlankChar: OrdinaryChar | '"' | '#' | '$' | '\'' | '_' | ';' | '[' | ']';
fragment TextLeadChar: OrdinaryChar | '"' | '#' | '$' | '\'' | '_' | ' ' | '\t' | '[' | ']';
fragment AnyPrintChar: OrdinaryChar | '"' | '#' | '$' | '\'' | '_' | ' ' | '\t' | ';' | '[' | ']';
fragment EOL: [\r\n];
fragment DIGIT: [0-9];

// Reserved Words
DATA_Name:          ('D' | 'd') ('A' | 'a') ('T' | 't') ('A' | 'a') '_' NonBlankChar+;
LOOP_:	            ('L' | 'l') ('O' | 'o') ('O' | 'o') ('P' | 'p') '_';
GLOBAL_:	          ('G' | 'g') ('L' | 'l') ('O' | 'o') ('B' | 'b') ('A' | 'a') ('L' | 'l') '_';
SAVE_:              ('S' | 's') ('A' | 'a') ('V' | 'v') ('E' | 'e') '_';
SAVE_Name:          ('S' | 's') ('A' | 'a') ('V' | 'v') ('E' | 'e') '_' NonBlankChar+;
STOP_:          	  ('S' | 's') ('T' | 't') ('O' | 'o') ('P' | 'p') '_';

// Whitespace & Comments
WhiteSpace:         [ \t\r\n]+ -> skip;
Comment:	          '#' AnyPrintChar*;

// Tags & Values
Tag:            	  '_' NonBlankChar+;
Value:          	  '.'
 |                  '?'
 |                  Numeric            { setText(std::string(DissolveSys::beforeChar(getText(), '('))); }
 |                  UnquotedString
 |                  SingleQuotedString { setText(getText().substr(1, getText().length()-2)); }
 |                  DoubleQuotedString { setText(getText().substr(1, getText().length()-2)); }
 |                  SemiColonTextField { setText(getText().find_first_not_of(";\r\n") != std::string::npos ? getText().substr(getText().find_first_not_of(";\r\n "), getText().find_last_not_of(";\r\n ") - getText().find_first_not_of(";\r\n ") + 1) : ""); }
 ;

// Numerics
Numeric:	          Number ( '(' UnsignedInteger ')' );
Number:	            Integer | Float;
Integer:	          ('+' | '-')? UnsignedInteger;
Float:              Integer Exponent
|                   ((DIGIT* '.' UnsignedInteger) | (DIGIT+ '.')) Exponent?;
Exponent:           ('e'|'E') ('+'|'-')? UnsignedInteger+;
UnsignedInteger:    DIGIT+;

// Character Strings
UnquotedString:     OrdinaryChar NonBlankChar*;
SingleQuotedString: '\'' AnyPrintChar*? '\'';
DoubleQuotedString: '"' AnyPrintChar*? '"';
SemiColonTextField: ';' EOL* (TextLeadChar AnyPrintChar* EOL*?)* ';';
