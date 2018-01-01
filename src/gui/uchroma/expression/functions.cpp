/*
	*** Expression Functions
	*** src/gui/uchroma/expression/functions.cpp
	Copyright T. Youngs 2010-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/expression/expression.h"
#include "gui/uchroma/expression/functions.h"
#include "gui/uchroma/expression/functionnode.h"
#include "math/constants.h"
#include <cmath>
#include <cstring>

// Static singleton
Functions functions;

// Default settings
bool Functions::assumeDegrees_ = false;

// Constructor
Functions::Functions()
{
	// Create pointer list
	initPointers();
}

// Constructor
Functions::~Functions()
{
}

/*
 * Settings
 */

// Set whether to assume values used in trig functions are degrees
void Functions::setAssumeDegrees(bool b)
{
	assumeDegrees_ = b;
}

// Return whether to assume values used in trig functions are degrees
bool Functions::assumeDegrees()
{
	return assumeDegrees_;
}

/*
 * Function List
 */

/* Argument Specification Tokens:
       Char	Meaning		Acceptable Types in VTypes
	N	Number		IntegerData, DoubleData
	*	<Repeat>	Any number of the last type again
	^	<Require Var>	Next token must be a modifiable variable and not a constant
	[]	<Cluster>	Surrounds groups of optional arguments that must be specified together
	|	<Or>		Separates alternative lists of arguments for the command
	&	<Array>		Next token must be an array
	/	<type Or>	Specifies an argument may be one of two types
	2-9	<NRepeat>	Next argument should occur N times
*/

// Command action
FunctionData Functions::data[Functions::nFunctions] = {

	// Operators
	{ "+",			"..",		true, "",
		"Internal Operator (+)" },
	{ "&&",			"..",		true, "",
		"Internal Operator (&&)" },
	{ "/",			"..",		true, "",
		"Internal Operator (/)" },
	{ "==",			"..",		true, "",
		"Internal Operator (==)" },
	{ "",			"..",		true, "",
		"Internal Operator (>)" },
	{ ">=",			"..",		true, "",
		"Internal Operator (>=)" },
	{ "<",			"..",		true, "",
		"Internal Operator (<)" },
	{ "<=",			"..",		true, "",
		"Internal Operator (<=)" },
	{ "%",			"..",		true, "",
		"Internal Operator (%)" },
	{ "*",			"..",		true, "",
		"Internal Operator (*)" },
	{ "-NEG",		".",		true, "",
		"Internal Operator (negate)" },
	{ "!",			"..",		true, "",
		"Internal Operator (!)" },
	{ "!=",			"..",		true, "",
		"Internal Operator (!=)" },
	{ "||",			"..",		true, "",
		"Internal Operator (||)" },
	{ "^",			"..",		true, "",
		"Internal Operator (^)" },
	{ "-",			"..",		true, "",
		"Internal Operator (-)" },

	// Flow control
	{ "*nofunction*",	"",		false, "",
		"" },
	{ "*joiner*",		"",		false, "",
		"" },
	{ "if",			"_",		false,
		"",
		"Perform a conditional test between the supplied expressions (or variables or constants)" },
	
	// Math Commands.
	{ "abs",		"N",		true,
		"double number",
		"Return absolute (i.e. positive) of value" },
	{ "acos",		"N",		true,
		"double cosx",
		"Return inverse cosine of supplied argument" },
	{ "asin",		"N",		true,
		"double sinx",
		"Return inverse sine of supplied argument" },
	{ "atan",		"N",		true,
		"double tanx",
		"Return inverse tangent of supplied argument" },
	{ "cos",		"N",		true,
		"double angle",
		"Return cosine of specified angle" },
	{ "exp",		"N",		true,
		"double value",
		"Return exponential of the argument" },
	{ "ln",			"N",		true,
		"double value",
		"Return natural (base-e) logarithm" },
	{ "log",		"N",		true,
		"double value",
		"Return base-10 logarithm" },
	{ "nint",		"N",		true,
		"double number",
		"Return nearest integer to supplied real value" },
	{ "sin",		"N",		true,
		"double angle",
		"Return sine of specified angle" },
	{ "sqrt",		"N",		true,
		"double number",
		"Return square root of number" },
	{ "tan",		"N",		true,
		"double angle",
		"Return tangent of specified angle" }
};

// Return enumerated command from string
Functions::Function Functions::function(const char* s)
{
	int result;
	for (result = Functions::NoFunction; result < Functions::nFunctions; result++) if (strcmp(data[result].keyword,s) == 0) break;
	return (Functions::Function) result;
}

/*
 * Operators
 */

// Add two quantities together
bool Functions::function_OperatorAdd(FunctionNode* c, double& rv)
{
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs + rhs;

	return true;
}

// Logical AND check on two operators
bool Functions::function_OperatorAnd(FunctionNode* c, double& rv)
{
	double v1, v2;
	if (!c->arg(0,v1)) return false;
	if (!c->arg(1,v2)) return false;

	rv = v1 && v2;

	return true;
}

// Divide one quantity by another
bool Functions::function_OperatorDivide(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs / rhs;

	return true;
}

// Equal To
bool Functions::function_OperatorEqualTo(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs == rhs;

	return true;
}

// Greater Than
bool Functions::function_OperatorGreaterThan(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs > rhs;

	return true;
}

// Greater Than Equal To
bool Functions::function_OperatorGreaterThanEqualTo(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs >= rhs;

	return true;
}

// Less Than
bool Functions::function_OperatorLessThan(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs < rhs;

	return true;
}

// Less Than Equal To
bool Functions::function_OperatorLessThanEqualTo(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs <= rhs;

	return true;
}

// Integer remainder of A/B
bool Functions::function_OperatorModulus(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = int(lhs) % int(rhs);

	return true;
}

// Multiply one quantity by another
bool Functions::function_OperatorMultiply(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs * rhs;

	return true;
}

// Negate value
bool Functions::function_OperatorNegate(FunctionNode* c, double& rv)
{
	if (!c->arg(0, rv)) return false;

	rv = -rv;

	return true;
}

// Not (Reverse Logic)
bool Functions::function_OperatorNot(FunctionNode* c, double& rv)
{
	if (!c->arg(0, rv)) return false;

	rv = (rv > 0 ? false : true);

	return true;
}

// Not Equal To
bool Functions::function_OperatorNotEqualTo(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs != rhs;

	return true;
}

// Logical OR check on two operators
bool Functions::function_OperatorOr(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs || rhs;

	return true;
}

// Raise one quantity to the power of another
bool Functions::function_OperatorPower(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = pow(lhs,rhs);

	return true;
}

// Subtract one quantity from another
bool Functions::function_OperatorSubtract(FunctionNode* c, double& rv)
{
	// Grab both argument (return) values and send them to be operated on
	double lhs, rhs;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;

	rv = lhs - rhs;

	return true;
}

/*
 * Flow Commands
 */

// Dummy Node
bool Functions::function_NoFunction(FunctionNode* c, double& rv)
{
	return true;
}

// Joiner
bool Functions::function_Joiner(FunctionNode* c, double& rv)
{
	// Execute both commands
	bool result = true;
	if (c->hasArg(0)) result = c->arg(0, rv);
	if (result && c->hasArg(1)) result = c->arg(1, rv);
	return result;
}

// If test
bool Functions::function_If(FunctionNode* c, double& rv)
{
	double ifval;
	if (!c->arg(0, ifval)) return false;
	if (ifval) return (c->arg(1, rv));
	else if (c->hasArg(2)) return (c->arg(2, rv));
	return true;
}

/*
 * Math Functions
 */

// Return absolute of argument
bool Functions::function_Abs(FunctionNode* c, double& rv)
{
	rv = fabs(c->argd(0));
	return true;
}

// Return invserse cosine of argument
bool Functions::function_ACos(FunctionNode* c, double& rv)
{
	rv = (assumeDegrees_ ? acos(c->argd(0)) * DEGRAD : acos(c->argd(0)) );
	return true;
}

// Return invserse sine of argument
bool Functions::function_ASin(FunctionNode* c, double& rv)
{
	rv = (assumeDegrees_ ? asin(c->argd(0)) * DEGRAD : asin(c->argd(0)) );
	return true;
}

// Return invserse tangent of argument
bool Functions::function_ATan(FunctionNode* c, double& rv)
{
	rv = (assumeDegrees_ ? atan(c->argd(0)) * DEGRAD : atan(c->argd(0)) );
	return true;
}

// Return cosine of argument (supplied in degrees)
bool Functions::function_Cos(FunctionNode* c, double& rv)
{
	rv = (assumeDegrees_ ? cos(c->argd(0) / DEGRAD) : cos(c->argd(0)) );
	return true;
}

// Return exponential of of argument
bool Functions::function_Exp(FunctionNode* c, double& rv)
{
	rv = exp(c->argd(0));
	return true;
}

// Return natural logarithm of argument
bool Functions::function_Ln(FunctionNode* c, double& rv)
{
	rv = log(c->argd(0));
	return true;
}

// Return base-10 logarithm of argument
bool Functions::function_Log(FunctionNode* c, double& rv)
{
	rv = log10(c->argd(0));
	return true;
}

// Round real value to nearest integer
bool Functions::function_Nint(FunctionNode* c, double& rv)
{
	rv = floor(c->argd(0) + 0.5);
	return true;
}

// Return sine of argument (supplied in degrees)
bool Functions::function_Sin(FunctionNode* c, double& rv)
{
	rv = (assumeDegrees_ ? sin(c->argd(0) / DEGRAD) : sin(c->argd(0)) );
	return true;
}

// Return square root of argument
bool Functions::function_Sqrt(FunctionNode* c, double& rv)
{
	rv = sqrt(c->argd(0));
	return true;
}

// Return tangent of argument (supplied in degrees)
bool Functions::function_Tan(FunctionNode* c, double& rv)
{
	rv = (assumeDegrees_ ? tan(c->argd(0) / DEGRAD) : tan(c->argd(0)) );
	return true;
}

/*
 * Commands
 */

// Return specified command keyword
const char* Functions::keyword(Functions::Function func)
{
	return Functions::data[func].keyword;
}

// Return specified command arguments
const char* Functions::arguments(Functions::Function func)
{
	return Functions::data[func].arguments;
}

// Return whether specified function returns a number
bool Functions::returnsNumber(Functions::Function func)
{
	return Functions::data[func].returnsNumber;
}

// Return specified command argument names
const char* Functions::argText(Functions::Function func)
{
	return Functions::data[func].argText;
}

// Return specified command syntax
const char* Functions::syntax(Functions::Function func)
{
	return Functions::data[func].syntax;
}

// Initialise Command Pointers
void Functions::initPointers()
{
	/*
	// Store pointers to all command functions
	*/
	// Operators
	pointers_[OperatorAdd] = &function_OperatorAdd;
	pointers_[OperatorAnd] = &function_OperatorAnd;
	pointers_[OperatorDivide] = &function_OperatorDivide;
	pointers_[OperatorEqualTo] = &function_OperatorEqualTo;
	pointers_[OperatorGreaterThan] = &function_OperatorGreaterThan;
	pointers_[OperatorGreaterThanEqualTo] = &function_OperatorGreaterThanEqualTo;
	pointers_[OperatorLessThan] = &function_OperatorLessThan;
	pointers_[OperatorLessThanEqualTo] = &function_OperatorLessThanEqualTo;
	pointers_[OperatorModulus] = &function_OperatorModulus;
	pointers_[OperatorMultiply] = &function_OperatorMultiply;
	pointers_[OperatorNegate] = &function_OperatorNegate;
	pointers_[OperatorNot] = &function_OperatorNot;
	pointers_[OperatorNotEqualTo] = &function_OperatorNotEqualTo;
	pointers_[OperatorOr] = &function_OperatorOr;
	pointers_[OperatorPower] = &function_OperatorPower;
	pointers_[OperatorSubtract] = &function_OperatorSubtract;

	pointers_[NoFunction] = &function_NoFunction;
	pointers_[Joiner] = &function_Joiner;

	// Flow control
	pointers_[If] = &function_If;

	// Math Commands
	pointers_[Abs] = &Functions::function_Abs;
	pointers_[ACos] = &Functions::function_ACos;
	pointers_[ASin] = &Functions::function_ASin;
	pointers_[ATan] = &Functions::function_ATan;
	pointers_[Cos] = &Functions::function_Cos;
	pointers_[Exp] = &Functions::function_Exp;
	pointers_[Ln] = &Functions::function_Ln;
	pointers_[Log] = &Functions::function_Log;
	pointers_[Nint] = &Functions::function_Nint;
	pointers_[Sin] = &Functions::function_Sin;
	pointers_[Sqrt] = &Functions::function_Sqrt;
	pointers_[Tan] = &Functions::function_Tan;
}

// Execute command
bool Functions::call(Functions::Function cf, FunctionNode* node, double& rv)
{
// 	printf("Calling command '%s' (node is %p)...\n", data[cf].keyword, node);
	return (this->pointers_[cf])(node, rv);
}
