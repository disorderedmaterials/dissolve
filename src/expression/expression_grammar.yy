/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "expression/expression.h"
#include "expression/functions.h"
#include "expression/variable.h"
#include "base/messenger.h"

/* Prototypes */
int ExpressionParser_lex(void);
void ExpressionParser_error(char *s);

%}

// Redeclare function names
%name-prefix="ExpressionParser_"

/* Type Definition */
%union {
	int functionId;				/* Function enum id */
	CharString* name;			/* Character pointer for names */
	ExpressionNode* node;			/* Expression node pointer */
	ExpressionVariable* variable;		/* Expression variable pointer */
	double doubleConst;			/* Constant double value */
};

%token <doubleConst> DISSOLVE_EXPR_CONSTANT
%token <name> DISSOLVE_EXPR_NEWTOKEN
%token <variable> DISSOLVE_EXPR_VAR
%token <functionId> DISSOLVE_EXPR_FUNCCALL
%token DISSOLVE_EXPR_IF
%nonassoc DISSOLVE_EXPR_ELSE

%left DISSOLVE_EXPR_AND DISSOLVE_EXPR_OR
%left '='
%left DISSOLVE_EXPR_GEQ DISSOLVE_EXPR_LEQ DISSOLVE_EXPR_EQ DISSOLVE_EXPR_NEQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%right DISSOLVE_EXPR_UMINUS
%right '!'
%right '^'

%type <node> constant expression expressionlist variable statement flowstatement statementlist block blockment
%type <node> function

%%

/* ------------------------- */
/* Main Program Construction */
/* ------------------------- */

/* Program List */
programlist:
	program						{ }
	| programlist program				{ }
	;

/* Single Program 'Statement' */
program:
	statementlist					{
		if (($1 != NULL) && (!Expression::target()->addStatement($1))) YYABORT;
		}
	| block						{
		if (($1 != NULL) && (!Expression::target()->addStatement($1))) YYABORT;
		}
	;

/* --------- */
/* Constants */
/* --------- */

constant:
	DISSOLVE_EXPR_CONSTANT					{
		$$ = Expression::target()->createConstant($1);
		}
	;

/* ----------------- */
/* Variables & Paths */
/* ----------------- */

/* Pre-Existing Variable */
variable:
	DISSOLVE_EXPR_VAR					{
		$$ = Expression::target()->addValueNode($1);
		if ($$ == NULL) YYABORT;
		}
	| variable '('					{
		Messenger::error("Tried to use a variable as a function.\n");
		YYABORT;
		}
	;

/* -------------- */
/* Function Calls */
/* -------------- */

/* Built-In Functions */
function:
	DISSOLVE_EXPR_FUNCCALL '(' ')'				{
		$$ = Expression::target()->addFunctionNode( (ExpressionFunctions::Function) $1);
		if ($$ == NULL) YYABORT;
		Messenger::printVerbose("PARSER: function : function '%s'\n", expressionFunctions.data[(ExpressionFunctions::Function) $1].keyword);
		}
	| DISSOLVE_EXPR_FUNCCALL '(' expressionlist ')'	{
		$$ = Expression::target()->addFunctionNodeWithArglist( (ExpressionFunctions::Function) $1, $3);
		if ($$ == NULL) YYABORT;
		Messenger::printVerbose("PARSER: function : function '%s' with exprlist\n", expressionFunctions.data[(ExpressionFunctions::Function) $1].keyword);
		}
	| DISSOLVE_EXPR_FUNCCALL error				{
		Messenger::error("Missing brackets after function call?\n");
		YYABORT;
		}
	;

/* ----------- */
/* Expressions */
/* ----------- */

/* Expression */
expression:
	constant					{ $$ = $1; }
	| function					{ $$ = $1; }
	| '-' expression %prec DISSOLVE_EXPR_UMINUS		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorNegate, $2); }
	| variable					{ $$ = $1; }
	| expression '+' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorAdd, $1, $3); }
	| expression '-' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorSubtract, $1, $3); }
	| expression '*' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorMultiply, $1, $3); }
	| expression '/' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorDivide, $1, $3); }
	| expression '^' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorPower, $1, $3); }
	| expression '%' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorModulus, $1, $3); }
	| expression DISSOLVE_EXPR_EQ expression		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorEqualTo, $1, $3); }
	| expression DISSOLVE_EXPR_NEQ expression		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorNotEqualTo, $1, $3); }
	| expression '>' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorGreaterThan, $1, $3); }
	| expression DISSOLVE_EXPR_GEQ expression		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorGreaterThanEqualTo, $1, $3); }
	| expression '<' expression			{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorLessThan, $1, $3); }
	| expression DISSOLVE_EXPR_LEQ expression		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorLessThanEqualTo, $1, $3); }
	| expression DISSOLVE_EXPR_AND expression		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorAnd, $1, $3); }
	| expression DISSOLVE_EXPR_OR expression		{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorOr, $1, $3); }
	| '(' expression ')'				{ $$ = $2; }
	| '!' expression				{ $$ = Expression::target()->addOperator(ExpressionFunctions::OperatorNot, $2); }
	| DISSOLVE_EXPR_NEWTOKEN				{ Messenger::error("'%s' has not been declared as a function or a variable.\n", (*yylval.name).get()); YYABORT; }
	;

/* Expression List */
expressionlist:
	expression					{
		$$ = $1;
		if ($$ == NULL) YYABORT;
		}
	| expressionlist ',' expression			{
		$$ = Expression::joinArguments($3,$1);
		}
	| expressionlist expression			{
		Messenger::error("Missing comma between items.\n");
		YYABORT;
		}
	;

/* ---------- */
/* Statements */
/* ---------- */

/* Statement Delimiter */
delim:
	';'						{ }
	| delim ';'					{ }
	;

/* Single Statement */
statement:
	expression delim				{
		$$ = $1;
		}
	| function delim				{
		$$ = $1;
		}
	| flowstatement					{
		$$ = $1;
		}
	;

/* Statement List */
statementlist:
	statement					{
		$$ = $1;
		}
	| statementlist statement			{
		if ($2 == NULL) $$ = $1;
		else $$ = Expression::target()->joinCommands($1, $2);
		}
	;

/* Block Statement */
block:
	'{' statementlist '}' {
		$$ = $2;
		}
	| '{' '}'					{
		$$ = Expression::target()->addFunctionNode(ExpressionFunctions::NoFunction);
		}
	;

/* Block or Statement, but not Statement List */
blockment:
	statement					{
		$$ = $1;
		}
	| block						{
		$$ = $1;
		}
	;

/* Flow-Control Statement */
flowstatement:
	DISSOLVE_EXPR_IF '(' expression ')' blockment DISSOLVE_EXPR_ELSE blockment 	{
		$$ = Expression::target()->addFunctionNode(ExpressionFunctions::If,$3,$5,$7);
		}
	| DISSOLVE_EXPR_IF '(' expression ')' blockment 			{
		$$ = Expression::target()->addFunctionNode(ExpressionFunctions::If,$3,$5);
		}
	;

%%

void yyerror(char *s)
{
}
