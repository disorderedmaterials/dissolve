// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

parser grammar TextPrimitiveParser;

options {
	tokenVocab = TextPrimitiveLexer;
}

// Parser file header
@parser::header {/* TextPrimitive ANTLR Parser */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@parser::postinclude { }

// Appears in the private part of the parser in the h file.
@parser::members { /* public parser declarations/members section */ }

// Appears in the public part of the parser in the h file.
@parser::declarations {/* private parser declarations section */}

// Appears in line with the other class member definitions in the cpp file.
@parser::definitions {/* parser definitions section */}

/*
 * TextPrimitive Grammar
 */

// Text Primitive
primitive: frag* EOF;

// Text Fragment
frag: Text                                          #plainText
| FormatterEscape OpenBrace frag* CloseBrace        #formatter
| SymbolEscape OpenBrace Symbol CloseBrace          #symbol
;

