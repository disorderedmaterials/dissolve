// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

lexer grammar TextPrimitiveLexer;

// Lexer file header
@lexer::header {/* TextPrimitive ANTLR Lexer */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@lexer::postinclude { 
    #include "gui/render/symbol.h"
    #include "gui/render/TextPrimitiveVisitor.h"
}

// Directly precedes the lexer class declaration in the h file (e.g. for additional types etc.).
@lexer::context {/* lexer context section */}

// Appears in the private part of the lexer in the h file.
@lexer::declarations {
    bool isFormatter(std::string text) { return TextPrimitiveVisitor::escapedFormatters().isValid(text.substr(1)); }
    bool isSymbol(std::string text) { return SymbolData::symbol(text) != SymbolData::nSymbols; }
}

// Appears in line with the other class member definitions in the cpp file.
@lexer::definitions {/* lexer definitions section */}

/*
 * Lexer Rules
 */

// Fragments
fragment LETTER: [a-zA-Z];
fragment ESCAPE: '\\';

// Symbols
OpenBrace: '{';
CloseBrace: '}';

// Escape Sequences
FormatterEscape: ESCAPE LETTER+ { isFormatter(getText()) }?;
SymbolEscape: ESCAPE 's' 'y' 'm';

// Enumerated Tokens
Symbol: LETTER+ { isSymbol(getText()) }?;

// Text
Text: (~([\\{}]))+;
