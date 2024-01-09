// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

parser grammar CIFImportParser;

options {
	tokenVocab = CIFImportLexer;
}

// Parser file header
@parser::header {/* CIFImport ANTLR Parser */}

// Add custom includes after standard ANTLR includes in both *.h and *.cpp files
@parser::postinclude { }

// Appears in the private part of the parser in the h file.
@parser::members { /* public parser declarations/members section */ }

// Appears in the public part of the parser in the h file.
@parser::declarations {/* private parser declarations section */}

// Appears in line with the other class member definitions in the cpp file.
@parser::definitions {/* parser definitions section */}

/*
 * CIFImport Grammar
 *
 * Rules defined here are based on those specified for CIF 1.1 (https://www.iucr.org/resources/cif/spec/version1.1/cifsyntax#bnf).
 * Some additional flexibility has been added, principally removing the enforced ordering of items to permit "looser" CIF files
 * (of which there are many) to be read.
 */

cif:                (Comment | dataBlockHeading | dataBlock)+?;

dataBlock:	        dataItems | saveFrame;

dataBlockHeading:   DATA_Name;

saveFrame:	        saveFrameHeading dataItems+ SAVE_;

saveFrameHeading:   SAVE_Name;

dataItems:          name=Tag value=Value                  #taggedData
|                   loop                                  #loopedData
;
loop:               LOOP_ columns+=Tag+ values+=Value+;
