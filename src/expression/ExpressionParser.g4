// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

parser grammar ExpressionParser;

options {
	tokenVocab = ExpressionLexer;
}

// Parser file header
@parser::header {/* Expression ANTLR Parser */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@parser::postinclude { }

// Appears in the private part of the parser in the h file.
@parser::members { /* public parser declarations/members section */ }

// Appears in the public part of the parser in the h file.
@parser::declarations {/* private parser declarations section */}

// Appears in line with the other class member definitions in the cpp file.
@parser::definitions {/* parser definitions section */}

/*
 * Expression Grammar
 */

// Compound expression
expression: expr EOF;

// Expressions
expr: OpenParenthesis expr CloseParenthesis                     					   #parentheses
| Subtract expr                                                 				       #unaryMinus
| expr Power expr                                               					   #power
| expr op=(Multiply | Divide) expr                              					   #multiplyDivide
| expr op=(Add | Subtract) expr                                 					   #addSubtract
| expr op=(LessEqual | GreaterEqual | Equal | NotEqual | LessThan | GreaterThan) expr  #comparison
| Name OpenParenthesis (expr (Comma expr)*)? CloseParenthesis   					   #function
| Name                                                          				   	   #variable
| value                                                         					   #number
;

// Numerical value
value: Integer
| Double;
