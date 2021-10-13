// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

// Main Structure - single node sequence
neta: RootNodes=nodeSequence? EOF;

// Node Sequence - Zero or more nodes, comma-separated
nodeSequence:    (Nodes+=node|Modifiers+=modifier|Options+=option|Flags+=flag) (Comma (Nodes+=node|Modifiers+=modifier|Options+=option|Flags+=flag))*      #sequence
| LHS=nodeSequence Or RHS=nodeSequence                                                                                                                     #orSequence
;

// Ring Node Sequence
ringSequence: ((Nodes+=ringOnlyNode|Modifiers+=modifier|Options+=option|Flags+=flag) (Comma (Nodes+=ringOnlyNode|Modifiers+=modifier|Options+=option|Flags+=flag))*)*;

// Common Nodes
node: bondCountNode
| characterNode
| connectionNode
| geometryNode
| hydrogenCountNode
| ringNode
;

// Ring-Specific Nodes
ringOnlyNode: presenceNode;

// Bond Count Node
bondCountNode: BondCountKeyword comparisonOperator Integer;

// Character Node
characterNode: Not? CharacterKeyword Targets=targetList;

// Connection Node
connectionNode: Not? ConnectionKeyword Targets=targetList OpenParenthesis Sequence=nodeSequence CloseParenthesis
| Not? ConnectionKeyword Targets=targetList
;

// Geometry Node
geometryNode: GeometryKeyword EqualityOperator geometry=Keyword;

// Hydrogen Count Node
hydrogenCountNode: HydrogenCountKeyword comparisonOperator Integer;

// Presence Node
presenceNode: Not? Targets=targetList OpenParenthesis Sequence=nodeSequence CloseParenthesis
| Not? Targets=targetList
;

// Ring Node
ringNode: Not? RingKeyword OpenParenthesis Sequence=ringSequence CloseParenthesis;

// Target List
elementOrType: Element
| FFTypeName
| FFTypeIndex;
targetList: targets+=elementOrType
| OpenSquareBracket targets+=elementOrType (Comma targets+=elementOrType)* CloseSquareBracket
;

// Contextual Modifiers (kwd op value)
modifier: Keyword comparisonOperator value=Integer;

// Option (kwd op kwd, only accepting '=' and '!=')
option: opt=Keyword comparisonOperator value=Keyword;

// Context Flags (kwd)
flag: Keyword;

// Comparison Operators
comparisonOperator: SizeOperator
| EqualityOperator
;
