/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */

#include "gui/render/textprimitive.h"
#include "base/messenger.h"

/* Prototypes */
int TextPrimitiveParser_lex(void);
void TextPrimitiveParser_error(char *s);

%}

// Redeclare function names
%name-prefix="TextPrimitiveParser_"

/* Type Definition */
%union {
	TextPrimitive::EscapeSequence escSeq;	/* Escape Sequence id */
	QString* text;				/* Text string */
	bool success;				/* TextFragment pointer */
};

%token <text> DISSOLVE_TEXT_TEXT
%token <escSeq> DISSOLVE_TEXT_ESCAPE
%token DISSOLVE_TEXT_FAIL

%type <success> fragment pushescape

%%

/* ------------------------------ */
/* Fragment Sequence Construction */
/* ------------------------------ */

/* Fragment Sequence */
fragmentsequence:
	fragment					{ }
	| fragmentsequence fragment			{ }
	;

/* ---------- */
/* Components */
/* ---------- */

fragment:
	DISSOLVE_TEXT_TEXT							{
		$$ = TextPrimitive::target()->addFragment(*$1);
		if (! $$) YYABORT;
		}
	| DISSOLVE_TEXT_ESCAPE pushescape '{' fragmentsequence '}' popescape	{
		$$ = $2;
		}
	| DISSOLVE_TEXT_FAIL							{
		YYABORT;
		}
	;

/* Semantics */

pushescape:
	/* empty */							{ $$ = TextPrimitive::target()->addEscape(yylval.escSeq); }
	;

popescape:
	/* empty */							{ TextPrimitive::target()->removeEscape(); }
	;

%%

void yyerror(char *s)
{
}
