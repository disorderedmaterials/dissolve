/*
	*** Expression Parser Lexer
	*** src/expression/expression_lexer.cpp
	Copyright T. Youngs 2015-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "expression/expression.h"
#include "expression/functions.h"
#include "expression/expression_grammar.hh"
#include "expression/variable.h"
#include "math/constants.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <math.h>
#include <cctype>

// Symbols
const char* SymbolTokenKeywords[Expression::nSymbolTokens] = { "==", ">=", "<=", "!=", "<>", "&&", "||" };
int SymbolTokenValues[Expression::nSymbolTokens] = { DISSOLVE_EXPR_EQ, DISSOLVE_EXPR_GEQ, DISSOLVE_EXPR_LEQ, DISSOLVE_EXPR_NEQ, DISSOLVE_EXPR_NEQ, DISSOLVE_EXPR_AND, DISSOLVE_EXPR_OR };
Expression::SymbolToken symbolToken(const char* s)
{
	for (int n=0; n<Expression::nSymbolTokens; ++n) if (DissolveSys::sameString(s, SymbolTokenKeywords[n])) return (Expression::SymbolToken) n;
	return Expression::nSymbolTokens;
}

/*
 * Bison Functions / Variables
 */

// Bison-generated ExpressionParser_lex()
int ExpressionParser_lex()
{
	if (!Expression::target()) return 0;
	return Expression::target()->lex();
}

// Parser lexer, called by yylex()
int Expression::lex()
{
	int n;
	bool done, hasExp;
	static CharString token, name;
	char c;
	token.clear();

	// Skip over whitespace
	while ((c = getChar()) == ' ' || c == '\t' || c == '\r' || c == '\n' );

	if (c == 0) return 0;

	// Set this point as the start of our new token (for error reporting)
	tokenStart_ = stringPos_-1;

	/*
	// Number Detection - Either '.' or  a digit begins a number
	*/
	if (c == '.' || isdigit(c))
	{
		Messenger::printVerbose("LEXER (%p): found the start of a number...\n", this);
		// Default to integer, unless first char is '.'
		hasExp = false;
		token += c;
		done = false;
		do
		{
			c = getChar();
			if (isdigit(c)) token += c;
			else if (c == '.') token += '.';
			else if ((c == 'e') || (c == 'E'))
			{
				// Check for previous exponential in number
				if (hasExp)
				{
					printf("Error: Number has two exponentiations (e/E).\n");
					return 0;
				}
				token += 'E';
				hasExp = true;
			}
			else if ((c == '-') || (c == '+'))
			{
				// We allow '-' or '+' only as part of an exponentiation, so if it is not preceeded by 'E' we stop parsing
				if ((!token.isEmpty()) && (!(token.lastChar() == 'E')))
				{
					unGetChar();
					done = true;
				}
				else token += c;
			}
			else
			{
				unGetChar();
				done = true;
			}
		} while (!done);
		// We now have the number as a text token...
		ExpressionParser_lval.doubleConst = token.asDouble();
		Messenger::printVerbose("LEXER (%p): found a numeric constant [%s] [%e]\n", this, token.get(), ExpressionParser_lval.doubleConst);
		return DISSOLVE_EXPR_CONSTANT;
	}

	/*
	// Alpha-token - function or variable
	*/
	if (isalpha (c))
	{
		do
		{
			token += c;
			c = getChar();
		}
		while (isalnum(c) || (c == '_'));
		unGetChar();
		Messenger::printVerbose("LEXER (%p): found an alpha token [%s]...\n", this, token.get());

		// Built-in numeric constants
		if (token == "Pi")
		{
			ExpressionParser_lval.doubleConst = PI;
			return DISSOLVE_EXPR_CONSTANT;
		}

		// Additional numeric constants
		if (useAdditionalConstants_)
		{
			if (token == "DEGRAD")
			{
				ExpressionParser_lval.doubleConst = DEGRAD;
				return DISSOLVE_EXPR_CONSTANT;
			}
			else if (token == "Bohr")
			{
				ExpressionParser_lval.doubleConst = BOHRRADIUS;
				return DISSOLVE_EXPR_CONSTANT;
			}
			else if (token == "NA")
			{
				ExpressionParser_lval.doubleConst = AVOGADRO;
				return DISSOLVE_EXPR_CONSTANT;
			}
			else if (token == "c")
			{
				ExpressionParser_lval.doubleConst = SPEEDOFLIGHT;
				return DISSOLVE_EXPR_CONSTANT;
			}
			else if (token == "kb")
			{
				ExpressionParser_lval.doubleConst = BOLTZMANN;
				return DISSOLVE_EXPR_CONSTANT;
			}
			else if (token == "h")
			{
				ExpressionParser_lval.doubleConst = PLANCK;
				return DISSOLVE_EXPR_CONSTANT;
			}
			else if (token == "hbar")
			{
				ExpressionParser_lval.doubleConst = HBAR;
				return DISSOLVE_EXPR_CONSTANT;
			}
		}

		// Is this a recognised high-level keyword?
		n = 0;
		if (token == "if") n = DISSOLVE_EXPR_IF;
		else if (token == "else") n = DISSOLVE_EXPR_ELSE;
		if (n != 0)
		{
			Messenger::printVerbose("LEXER (%p): ...which is a high-level keyword (%i)\n",this,n);
			return n;
		}

		// Is it an existing variable?
		ExpressionVariable* v = variable(token);
		if (v != NULL)
		{
			Messenger::printVerbose("LEXER (%p): ...which is an existing variable (->VAR)\n", this);
			ExpressionParser_lval.variable = v;
			return DISSOLVE_EXPR_VAR;
		}

		// Is it a known function keyword?
		n = ExpressionFunctions::function(token.get());
		if (n != ExpressionFunctions::nFunctions)
		{
			Messenger::printVerbose("LEXER (%p): ... which is a function (->FUNCCALL).\n", this);
			ExpressionParser_lval.functionId = n;
			functionStart_ = tokenStart_;
			return DISSOLVE_EXPR_FUNCCALL;
		}

		// If we get to here then we have found an unrecognised alphanumeric token
		// If the 'generateMissingVariables_' flag is set, generate a new variable with the current token name
		if (generateMissingVariables_)
		{
			Messenger::printVerbose("LEXER (%p): ...which has been autogenerated as a variable (->VAR)\n", this);
			ExpressionParser_lval.variable = createVariable(token);
			return DISSOLVE_EXPR_VAR;
		}
		else
		{
			Messenger::printVerbose("LEXER (%p): ...which is unrecognised (->NEWTOKEN)\n", this);
			name = token;
			ExpressionParser_lval.name = &name;
			return DISSOLVE_EXPR_NEWTOKEN;
		}
	}

	// We have found a symbolic character (or a pair) that corresponds to an operator
	// Return immediately in the case of brackets, comma, and semicolon
	if ((c == '(') || (c == ')') || (c == ';') || (c == ',') || (c == '{') || (c == '}') || (c == '[') || (c == ']') || (c == '%') || (c == ':'))
	{
		Messenger::printVerbose("LEXER (%p): found symbol [%c]\n",this,c);
		return c;
	}
	token += c;

	// Similarly, if the next character is a period, bracket or double quotes, return immediately
	char c2 = peekChar();
	if ((c2 == '.') || (c2 == '(') || (c2 == ')') || (c2 == ';') || (c2 == '{') || (c2 == '}') || (c2 == '"')) return c;

	// If next character is '-', return now if previous char was *not* another '-'
	if ((c2 == '-') && (c != '-')) return c;

	// If it is 'punctuation', add this second character to our operator and search for it
	if (ispunct(c2))
	{
		c = getChar();
		token += c;
		Messenger::printVerbose("LEXER (%p): found symbol [%s]\n", this, token.get());
		SymbolToken st = symbolToken(token.get());
		if (st != nSymbolTokens) return SymbolTokenValues[st];
		else printf("Error: Unrecognised symbol found in input (%s).\n", token.get());
 	}
	else
	{
		// Make sure that this is a known symbol
		if ((c == '$') || (c == '%') || (c == '&') || (c == '@') || (c == '?') || (c == ':'))
		{
			printf("Error: Unrecognised symbol found in input (%c).\n", c);
		}
		else return c;
	}

	return 0;
}
