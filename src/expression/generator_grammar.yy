/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "expression/expression.h"
#include "expression/generator.h"
#include "expression/functions.h"
#include "expression/variable.h"
#include "base/messenger.h"

/* Prototypes */
int ExpressionGenerator_lex(void);
void ExpressionGenerator_error(char *s);

%}

// Redeclare function names
%name-prefix="ExpressionGenerator_"

/* Type Definition */
%union {
	int functionId;				/* Function enum id */
	std::string *name;			/* String pointer for names */
	ExpressionNode* node;			/* Expression node pointer */
	ExpressionVariable* variable;		/* Expression variable pointer */
	int integerConst;			/* Constant integer value */
	double doubleConst;			/* Constant double value */
};

%token <integerConst> DISSOLVE_EXPR_INTEGERCONSTANT
%token <doubleConst> DISSOLVE_EXPR_DOUBLECONSTANT
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
		if (($1 != NULL) && (!ExpressionGenerator::expression()->addStatement($1))) YYABORT;
		}
	| block						{
		if (($1 != NULL) && (!ExpressionGenerator::expression()->addStatement($1))) YYABORT;
		}
	;

/* --------- */
/* Constants */
/* --------- */

constant:
	DISSOLVE_EXPR_INTEGERCONSTANT					{
		$$ = ExpressionGenerator::expression()->createConstant($1);
		}
	| DISSOLVE_EXPR_DOUBLECONSTANT					{
		$$ = ExpressionGenerator::expression()->createConstant($1);
		}
	;

/* ----------------- */
/* Variables & Paths */
/* ----------------- */

/* Pre-Existing Variable */
variable:
	DISSOLVE_EXPR_VAR					{
		$$ = ExpressionGenerator::expression()->addValueNode($1);
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
		$$ = ExpressionGenerator::expression()->addFunctionNode( (ExpressionFunctions::Function) $1);
		if ($$ == NULL) YYABORT;
		Messenger::printVerbose("PARSER: function : function '{}'\n", expressionFunctions.data[(ExpressionFunctions::Function) $1].keyword);
		}
	| DISSOLVE_EXPR_FUNCCALL '(' expressionlist ')'	{
		$$ = ExpressionGenerator::expression()->addFunctionNodeWithArglist( (ExpressionFunctions::Function) $1, $3);
		if ($$ == NULL) YYABORT;
		Messenger::printVerbose("PARSER: function : function '{}' with exprlist\n", expressionFunctions.data[(ExpressionFunctions::Function) $1].keyword);
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
	| '-' expression %prec DISSOLVE_EXPR_UMINUS		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorNegate, $2); }
	| variable					{ $$ = $1; }
	| expression '+' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorAdd, $1, $3); }
	| expression '-' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorSubtract, $1, $3); }
	| expression '*' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorMultiply, $1, $3); }
	| expression '/' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorDivide, $1, $3); }
	| expression '^' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorPower, $1, $3); }
	| expression '%' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorModulus, $1, $3); }
	| expression DISSOLVE_EXPR_EQ expression		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorEqualTo, $1, $3); }
	| expression DISSOLVE_EXPR_NEQ expression		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorNotEqualTo, $1, $3); }
	| expression '>' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorGreaterThan, $1, $3); }
	| expression DISSOLVE_EXPR_GEQ expression		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorGreaterThanEqualTo, $1, $3); }
	| expression '<' expression			{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorLessThan, $1, $3); }
	| expression DISSOLVE_EXPR_LEQ expression		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorLessThanEqualTo, $1, $3); }
	| expression DISSOLVE_EXPR_AND expression		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorAnd, $1, $3); }
	| expression DISSOLVE_EXPR_OR expression		{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorOr, $1, $3); }
	| '(' expression ')'				{ $$ = $2; }
	| '!' expression				{ $$ = ExpressionGenerator::expression()->addOperator(ExpressionFunctions::OperatorNot, $2); }
	| DISSOLVE_EXPR_NEWTOKEN				{ Messenger::error("'{}' has not been declared as a function or a variable.\n", *yylval.name); YYABORT; }
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
		else $$ = ExpressionGenerator::expression()->joinCommands($1, $2);
		}
	;

/* Block Statement */
block:
	'{' statementlist '}' {
		$$ = $2;
		}
	| '{' '}'					{
		$$ = ExpressionGenerator::expression()->addFunctionNode(ExpressionFunctions::NoFunction);
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
		$$ = ExpressionGenerator::expression()->addFunctionNode(ExpressionFunctions::If,$3,$5,$7);
		}
	| DISSOLVE_EXPR_IF '(' expression ')' blockment 			{
		$$ = ExpressionGenerator::expression()->addFunctionNode(ExpressionFunctions::If,$3,$5);
		}
	;

%%

void yyerror(char *s)
{
}
