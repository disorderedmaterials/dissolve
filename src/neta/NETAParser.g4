// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

parser grammar NETAParser;

options {
	tokenVocab = NETALexer;
}

// Parser file header
@parser::header {/* NETA ANTLR Parser */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@parser::postinclude { }

// Appears in the private part of the parser in the h file.
@parser::members {
/* public parser declarations/members section */
}

// Appears in the public part of the parser in the h file.
@parser::declarations {/* private parser declarations section */}

// Appears in line with the other class member definitions in the cpp file.
@parser::definitions {/* parser definitions section */}

/*
 * NETA Grammar
 */

// Main Structure - Zero or more nodes, comma-separated
neta: nodeSequence EOF;

// Node Sequence
nodeSequence: (node (Comma node)*)*;

// Any Node
node: commonNode
| orNode
| contextual;

// 'Or' Node
orNode: commonNode Or commonNode;

// Common Nodes
commonNode: connectionNode
| ringNode;

// Contextuals
contextual: modifier
| flag;

// Ring Node Sequence
ringNodeSequence: (ringOnlyNode (Comma ringOnlyNode)*)*;

// Ring-Specific Nodes
ringOnlyNode: presenceNode
| modifier
| flag;

// Connection Node
connectionNode: ConnectionKeyword targetList OpenParenthesis nodeSequence CloseParenthesis
| ConnectionKeyword targetList;

// Presence Node
presenceNode: targetList OpenParenthesis nodeSequence CloseParenthesis
| targetList;

// Ring Node
ringNode: RingKeyword OpenParenthesis ringNodeSequence CloseParenthesis;

// Target List
elementOrType: Element
| FFTypeName
| FFTypeIndex;
targetList: targets+=elementOrType
| OpenSquareBracket targets+=elementOrType (Comma targets+=elementOrType)* CloseSquareBracket;

// Contextual Modifiers (kwd op value)
modifier: Keyword ComparisonOperator value=Integer;

// Context Flags (kwd)
flag: Keyword;
