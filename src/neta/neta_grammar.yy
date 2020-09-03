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
#include "neta/presence.h"
#include "neta/ring.h"
#include "base/messenger.h"
#include "templates/reflist.h"

/* Prototypes */
int NETADefinitionGenerator_lex(void);
void NETADefinitionGenerator_error(char *s);

// Local Variables
std::string localName;

%}

// Redeclare function names
%name-prefix="NETADefinitionGenerator_"

/* Type Definition */
%union {
	void* atomTargetDummy;				/* Dummy Type for Atom Targets*/
	int elementZ;					/* Element Z */
	std::string *name;				/* Character pointer for names */
	NETANode* node;					/* NETADefinition node pointer */
	NETANode::ComparisonOperator valueOperator;	/* Comparison Operator */
	int integerConst;				/* Constant integer value */
	double doubleConst;				/* Constant double value */
};

%token <integerConst> DISSOLVE_NETA_INTEGERCONSTANT
%token <doubleConst> DISSOLVE_NETA_DOUBLECONSTANT
%token <elementZ> DISSOLVE_NETA_ELEMENT
%token <valueOperator> DISSOLVE_NETA_OPERATOR
%token <name> DISSOLVE_NETA_MODIFIER DISSOLVE_NETA_FLAG DISSOLVE_NETA_NAME
%token DISSOLVE_NETA_RING DISSOLVE_NETA_UNKNOWNTOKEN

%left DISSOLVE_NETA_AND DISSOLVE_NETA_OR
%left '='
%left DISSOLVE_NETA_GEQ DISSOLVE_NETA_LEQ DISSOLVE_NETA_EQ DISSOLVE_NETA_NEQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%right DISSOLVE_NETA_UMINUS
%right '!'
%right '^'

%type <node> node nodeSequence ringNode ringNodeSequence createPresenceNode createConnectionNode createRingNode
%type <valueOperator> valueOperator
%type <atomTargetDummy> target targets targetList

%%

/* ----------------------- */
/* NETA Definition Grammar */
/* ----------------------- */

/* Main Structure */
neta:
	/* empty */					{ YYACCEPT; }
	| nodeSequence					{ YYACCEPT; }
	;

/* Sequence of Nodes */
nodeSequence:
	node						{ $$ = $1; }
	| '!' node					{ $2->setReverseLogic(); $$ = $2; }
	| nodeSequence ',' node				{ $$ = $3; }
// 	| nodeSequence '|' nodeSequence			{ $$ = NETADefinitionGenerator::context()->joinWithLogic($1, NETALogicNode::OrLogic, $3); }
	;

/* Sequence of Nodes allowable for a Ring */
ringNodeSequence:
	ringNode					{ $$ = $1; }
	| ringNodeSequence ',' ringNode			{ $$ = $3; }
	;

ringNode:
	targetList createPresenceNode							{ $$ = $2; }
	| targetList createPresenceNode	pushContext '(' nodeSequence ')' popContext	{ $$ = $2; }
	| contextModifier								{ $$ = NULL; }
	;

/* Nodes */
node:
	'-' targetList createConnectionNode							{ $$ = $3; }
	| '-' targetList createConnectionNode pushContext '(' nodeSequence ')' popContext	{ $$ = $3; }
	| DISSOLVE_NETA_RING createRingNode pushContext '(' ringNodeSequence ')' popContext	{ $$ = $2; }
	| contextModifier									{ $$ = NULL; }
	| contextFlag										{ $$ = NULL; }
	| DISSOLVE_NETA_UNKNOWNTOKEN								{ YYABORT; }
	;

/* Target Element / Type Generic */
targetList:
	target						{ $$ = NULL; }
	| '[' targets ']'				{ $$ = NULL; }
	;

/* Target Elements / Types */
targets:
	target						{ $$ = NULL; }
	| targets ',' target				{ $$ = NULL; }
	;

/* Target Element / Type */
target:
	DISSOLVE_NETA_ELEMENT				{ if (!NETADefinitionGenerator::addElementTarget($1)) YYABORT; $$ = NULL; }
	| '&' setExpectName DISSOLVE_NETA_INTEGERCONSTANT unsetExpectName		{ if (!NETADefinitionGenerator::addAtomTypeTarget($3)) YYABORT; $$ = NULL; }
	| '&' setExpectName DISSOLVE_NETA_NAME unsetExpectName		{ if (!NETADefinitionGenerator::addAtomTypeTarget(*yylval.name)) YYABORT; $$ = NULL; }
	;

/* Context Modifiers */
contextModifier:
	DISSOLVE_NETA_MODIFIER storeName valueOperator DISSOLVE_NETA_INTEGERCONSTANT	{ if (!NETADefinitionGenerator::context()->setModifier(localName, $3, $4)) YYABORT; }
	;

/* Context flags */
contextFlag:
	DISSOLVE_NETA_FLAG storeName			{ if (!NETADefinitionGenerator::context()->setFlag(localName, true)) YYABORT; }
	;

/* Operators */
valueOperator:
	DISSOLVE_NETA_OPERATOR				{ $$ = yylval.valueOperator; }
	;

/* Node Creation */
createPresenceNode:
	/* empty */					{ $$ = NETADefinitionGenerator::context()->createPresenceNode(NETADefinitionGenerator::targetElements(), NETADefinitionGenerator::targetAtomTypes()); NETADefinitionGenerator::clearTargets(); }
	;
createConnectionNode:
	/* empty */					{ $$ = NETADefinitionGenerator::context()->createConnectionNode(NETADefinitionGenerator::targetElements(), NETADefinitionGenerator::targetAtomTypes()); NETADefinitionGenerator::clearTargets(); }
	;
createRingNode:
	/* empty */					{ $$ = NETADefinitionGenerator::context()->createRingNode(); }
	;

/* Context Management */
pushContext:
	/* empty */					{ NETADefinitionGenerator::pushContext(); }
	;
popContext:
	/* empty */					{ NETADefinitionGenerator::popContext(); }
	;
storeName:
	/* empty */					{ localName = *yylval.name; }
	;
setExpectName:
	/* empty */					{ NETADefinitionGenerator::setExpectName(true); }
	;
unsetExpectName:
	/* empty */					{ NETADefinitionGenerator::setExpectName(false); }
	;

%%

void yyerror(char *s)
{
}
