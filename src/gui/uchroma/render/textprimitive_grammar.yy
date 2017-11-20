/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */

#include "render/textprimitive.h"
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

%token <text> UCR_TP_TEXT
%token <escSeq> UCR_TP_ESCAPE
%token UCR_TP_FAIL

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
	UCR_TP_TEXT							{
		$$ = TextPrimitive::target()->addFragment(*$1);
		if (! $$) YYABORT;
		}
	| UCR_TP_ESCAPE pushescape '{' fragmentsequence '}' popescape	{
		$$ = $2;
		}
	| UCR_TP_FAIL							{
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
