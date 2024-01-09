// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
nodeSequence:    (Nodes+=node|Modifiers+=modifier|Options+=option|Flags+=flag|Identifiers+=identifier) (Comma (Nodes+=node|Modifiers+=modifier|Options+=option|Flags+=flag|Identifiers+=identifier))*      #sequence
| LHS=nodeSequence Or RHS=nodeSequence                                                                                                                     #orSequence
;

// Ring Node Sequence
ringSequence: ((Nodes+=ringAtomNode|Modifiers+=modifier|Options+=option|Flags+=flag) (Comma (Nodes+=ringAtomNode|Modifiers+=modifier|Options+=option|Flags+=flag))*)*;

// Common Nodes
node: bondCountNode
| characterNode
| connectionNode
| geometryNode
| hydrogenCountNode
| ringNode
| subSequence
;

// Bracketed node sub-sequence
subSequence: Not? OpenParenthesis Sequence=nodeSequence CloseParenthesis;

// Bond Count Node
bondCountNode: BondCountKeyword comparisonOperator Integer;

// Character Node
characterNode: Not? CharacterKeyword Targets=targetList;

// Connection Node
connectionNode: Not? ConnectionKeyword Targets=targetList OpenParenthesis Sequence=nodeSequence CloseParenthesis
| Not? ConnectionKeyword Targets=targetList
;

// Geometry Node
geometryNode: GeometryKeyword EqualityOperator geometry=Text;

// Hydrogen Count Node
hydrogenCountNode: HydrogenCountKeyword comparisonOperator Integer;

// Ring Node
ringNode: Not? RingKeyword OpenParenthesis Sequence=ringSequence CloseParenthesis;

// Ring Atom Node
ringAtomNode: Not? Targets=targetList OpenParenthesis Sequence=nodeSequence CloseParenthesis
| Not? Targets=targetList
;

// Target List
elementOrType: Element
| FFTypeName
| FFTypeIndex;
targetList: targets+=elementOrType
| OpenSquareBracket targets+=elementOrType (Comma targets+=elementOrType)* CloseSquareBracket
;

// Contextual Modifiers (kwd op value)
modifier: Text comparisonOperator value=Integer;

// Option (kwd op kwd, only accepting '=' and '!=')
option: opt=Text comparisonOperator value=Text;

// Context Flags (kwd)
flag: Text;

// Identifying Name
identifier: IdKeyword names+=Text
| IdKeyword OpenSquareBracket names+=Text (Comma names+=Text)* CloseSquareBracket;

// Comparison Operators
comparisonOperator: SizeOperator
| EqualityOperator
;
