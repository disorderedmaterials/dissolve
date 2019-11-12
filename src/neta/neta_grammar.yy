/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "neta/generator.h"
#include "neta/connection.h"
#include "neta/neta.h"
#include "base/messenger.h"

/* Prototypes */
int NETADefinitionGenerator_lex(void);
void NETADefinitionGenerator_error(char *s);

%}

// Redeclare function names
%name-prefix="NETADefinitionGenerator_"

/* Type Definition */
%union {
	void* atomTargetDummy;			/* Dummy Type for Atom Targets*/
	int elementZ;				/* Element Z */
	CharString* name;			/* Character pointer for names */
	NETANode* node;				/* NETADefinition node pointer */
	int integerConst;			/* Constant integer value */
	double doubleConst;			/* Constant double value */
};

%token <integerConst> DISSOLVE_NETA_INTEGERCONSTANT
%token <doubleConst> DISSOLVE_NETA_DOUBLECONSTANT
%token <elementZ> DISSOLVE_NETA_ELEMENT
%token DISSOLVE_NETA_UNKNOWNTOKEN

%left DISSOLVE_NETA_AND DISSOLVE_NETA_OR
%left '='
%left DISSOLVE_NETA_GEQ DISSOLVE_NETA_LEQ DISSOLVE_NETA_EQ DISSOLVE_NETA_NEQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%right DISSOLVE_NETA_UMINUS
%right '!'
%right '^'

// %type <node> constant expression expressionlist variable statement flowstatement statementlist block blockment
// %type <node> function

%type <node> node nodesequence
%type <atomTargetDummy> target targets targetlist

%%

/* ----------------------- */
/* NETA Definition Grammar */
/* ----------------------- */

/* Main Structure */
neta:
	/* empty */					{ YYACCEPT; }
	| nodesequence					{ NETADefinitionGenerator::definition()->setRootNode($1); YYACCEPT; }
	;

/* Sequence of Nodes */
nodesequence:
	node						{ $$ = $1; }
	| '!' node					{ $2->setReverseLogic(); $$ = $2; }
	| nodesequence ',' node				{ $$ = NETADefinitionGenerator::definition()->joinWithLogic($1, NETALogicNode::AndLogic, $3); }
	| nodesequence '|' nodesequence			{ $$ = NETADefinitionGenerator::definition()->joinWithLogic($1, NETALogicNode::OrLogic, $3); }
	| '(' nodesequence ')'				{ $$ = $2; }
	;

/* Nodes */
node:
	'-' targetlist					{ $$ = NETADefinitionGenerator::definition()->createConnectionNode(NETADefinitionGenerator::targetElements(), NETADefinitionGenerator::targetAtomTypes()); }
	| DISSOLVE_NETA_UNKNOWNTOKEN			{ YYABORT; }
	;

/* Target Element / Type Generic */
targetlist:
	target						{ $$ = NULL; }
	| '[' target ']'				{ $$ = NULL; }
	| '[' targets ']'				{ $$ = NULL; }
	;

/* Target Elements / Types */
targets:
	target						{ $$ = NULL; }
	| targets ',' target				{ $$ = NULL; }
	;

/* Target Element / Type */
target:
	DISSOLVE_NETA_ELEMENT				{ NETADefinitionGenerator::addTarget($1); $$ = NULL; }
	| '&' DISSOLVE_NETA_INTEGERCONSTANT		{ NETADefinitionGenerator::addTarget(-$2); $$ = NULL; }
	;

%%

void yyerror(char *s)
{
}
