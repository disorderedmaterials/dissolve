/*
    *** Expression Functions
    *** src/expression/functions.cpp
    Copyright T. Youngs 2010-2020

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

#include "expression/functions.h"
#include "base/sysfunc.h"
#include "expression/expression.h"
#include "expression/function.h"
#include "math/constants.h"
#include "math/mathfunc.h"
#include <cmath>
#include <cstring>

// Static singleton
ExpressionFunctions expressionFunctions;

// Default settings
bool ExpressionFunctions::assumeDegrees_ = true;

ExpressionFunctions::ExpressionFunctions()
{
    // Create pointer list
    initPointers();
}

ExpressionFunctions::~ExpressionFunctions() {}

/*
 * Settings
 */

// Set whether to assume values used in trig functions are degrees
void ExpressionFunctions::setAssumeDegrees(bool b) { assumeDegrees_ = b; }

// Return whether to assume values used in trig functions are degrees
bool ExpressionFunctions::assumeDegrees() { return assumeDegrees_; }

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
ExpressionFunctionData ExpressionFunctions::data[ExpressionFunctions::nFunctions] = {

    // Operators
    {"+", 0, "", "Operator (+)"},
    {"&&", 0, "", "Operator (&&)"},
    {"/", 0, "", "Operator (/)"},
    {"==", 0, "", "Operator (==)"},
    {"", 0, "", "Operator (>)"},
    {">=", 0, "", "Operator (>=)"},
    {"<", 0, "", "Operator (<)"},
    {"<=", 0, "", "Operator (<=)"},
    {"%", 0, "", "Operator (%)"},
    {"*", 0, "", "Operator (*)"},
    {"-NEG", 0, "", "Operator (negate)"},
    {"!", 0, "", "Operator (!)"},
    {"!=", 0, "", "Operator (!=)"},
    {"||", 0, "", "Operator (||)"},
    {"^", 0, "", "Operator (^)"},
    {"-", 0, "", "Operator (-)"},

    // Flow control
    {"*nofunction*", 0, "", ""},
    {"*joiner*", 0, "", ""},
    {"if", 0, "", "Perform a conditional test between the supplied expressions (or variables or constants)"},

    // Math Commands.
    {"abs", 1, "double number", "Return absolute (i.e. positive) of value"},
    {"acos", 1, "double cosx", "Return inverse cosine of supplied argument"},
    {"asin", 1, "double sinx", "Return inverse sine of supplied argument"},
    {"atan", 1, "double tanx", "Return inverse tangent of supplied argument"},
    {"cos", 1, "double angle", "Return cosine of specified angle"},
    {"exp", 1, "double value", "Return exponential of the argument"},
    {"ln", 1, "double value", "Return natural (base-e) logarithm"},
    {"log", 1, "double value", "Return base-10 logarithm"},
    {"nint", 1, "double number", "Return nearest integer to supplied real value"},
    {"sin", 1, "double angle", "Return sine of specified angle"},
    {"sqrt", 1, "double number", "Return square root of number"},
    {"tan", 1, "double angle", "Return tangent of specified angle"}};

// Return enumerated command from string
ExpressionFunctions::Function ExpressionFunctions::function(std::string_view s)
{
    for (int result = ExpressionFunctions::NoFunction; result < ExpressionFunctions::nFunctions; result++)
        if (DissolveSys::sameString(data[result].keyword, s))
            return (ExpressionFunctions::Function)result;

    return (ExpressionFunctions::Function)ExpressionFunctions::nFunctions;
}

/*
 * Operators
 */

// Add two quantities together
bool ExpressionFunctions::function_OperatorAdd(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = a.asInteger() + b.asInteger();
    else
        result = a.asDouble() + b.asDouble();

    return true;
}

// Logical AND check on two operators
bool ExpressionFunctions::function_OperatorAnd(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    auto aa = a.isInteger() ? a.asInteger() > 0 : a.asDouble() > 0.0;
    auto bb = b.isInteger() ? b.asInteger() > 0 : b.asDouble() > 0.0;

    result = (aa && bb ? 1 : 0);

    return true;
}

// Divide one quantity by another
bool ExpressionFunctions::function_OperatorDivide(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = a.asInteger() / b.asInteger();
    else
        result = a.asDouble() / b.asDouble();

    return true;
}

// Equal To
bool ExpressionFunctions::function_OperatorEqualTo(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = (a.asInteger() == b.asInteger() ? 1 : 0);
    else
        result = (a.asDouble() == b.asDouble() ? 1 : 0);

    return true;
}

// Greater Than
bool ExpressionFunctions::function_OperatorGreaterThan(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = a.asInteger() + b.asInteger();
    else
        result = a.asDouble() + b.asDouble();

    return true;
}

// Greater Than Equal To
bool ExpressionFunctions::function_OperatorGreaterThanEqualTo(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = (a.asInteger() >= b.asInteger() ? 1 : 0);
    else
        result = (a.asDouble() >= b.asDouble() ? 1 : 0);

    return true;
}

// Less Than
bool ExpressionFunctions::function_OperatorLessThan(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = (a.asInteger() < b.asInteger() ? 1 : 0);
    else
        result = (a.asDouble() < b.asDouble() ? 1 : 0);

    return true;
}

// Less Than Equal To
bool ExpressionFunctions::function_OperatorLessThanEqualTo(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = (a.asInteger() <= b.asInteger() ? 1 : 0);
    else
        result = (a.asDouble() <= b.asDouble() ? 1 : 0);

    return true;
}

// Module of A/B
bool ExpressionFunctions::function_OperatorModulus(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = a.asInteger() & b.asInteger();
    else
        result = fmod(a.asDouble(), b.asDouble());

    return true;
}

// Multiply A with B
bool ExpressionFunctions::function_OperatorMultiply(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = a.asInteger() * b.asInteger();
    else
        result = a.asDouble() * b.asDouble();

    return true;
}

// Negate value
bool ExpressionFunctions::function_OperatorNegate(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a;

    // Retrieve the argument
    if (!c->arg(0, a))
        return false;

    if (a.isInteger())
        result = -a.asInteger();
    else
        result = -a.asDouble();

    return true;
}

// Not (Reverse Logic)
bool ExpressionFunctions::function_OperatorNot(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a;

    // Retrieve the argument
    if (!c->arg(0, a))
        return false;

    if (a.isInteger())
        result = (a.asInteger() > 0 ? 1 : 0);
    else if (a.isDouble())
        result = (a.asDouble() > 0.0 ? 1 : 0);

    return true;
}

// Not Equal To
bool ExpressionFunctions::function_OperatorNotEqualTo(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = (a.asInteger() != b.asInteger() ? 1 : 0);
    else
        result = (a.asDouble() != b.asDouble() ? 1 : 0);

    return true;
}

// Logical OR check on two operators
bool ExpressionFunctions::function_OperatorOr(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    auto aa = a.isInteger() ? a.asInteger() > 0 : a.asDouble() > 0.0;
    auto bb = b.isInteger() ? b.asInteger() > 0 : b.asDouble() > 0.0;

    result = (aa || bb ? 1 : 0);

    return true;
}

// Raise one quantity to the power of another
bool ExpressionFunctions::function_OperatorPower(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = DissolveMath::power(a.asInteger(), b.asInteger());
    else
        result = pow(a.asDouble(), b.asDouble());

    return true;
}

// Subtract one quantity from another
bool ExpressionFunctions::function_OperatorSubtract(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a, b;

    // Retrieve the two arguments
    if (!c->arg(0, a))
        return false;
    if (!c->arg(1, b))
        return false;

    if (ExpressionValue::bothIntegers(a, b))
        result = a.asInteger() - b.asInteger();
    else
        result = a.asDouble() - b.asDouble();

    return true;
}

/*
 * Flow Commands
 */

// Dummy Node
bool ExpressionFunctions::function_NoFunction(ExpressionFunction *c, ExpressionValue &result) { return true; }

// Joiner
bool ExpressionFunctions::function_Joiner(ExpressionFunction *c, ExpressionValue &result)
{
    // Execute both commands
    auto success = true;

    if (c->hasArg(0))
        success = c->arg(0, result);
    if (success && c->hasArg(1))
        success = c->arg(1, result);

    return success;
}

// If test
bool ExpressionFunctions::function_If(ExpressionFunction *c, ExpressionValue &result)
{
    // Retrieve result of if test
    ExpressionValue ifval;
    if (!c->arg(0, ifval))
        return false;

    if (ifval.isInteger() ? ifval.asInteger() > 0 : ifval.asDouble() > 0.0)
        return (c->arg(1, result));
    else if (c->hasArg(2))
        return (c->arg(2, result));

    return true;
}

/*
 * Math Functions
 */

// Return absolute of argument
bool ExpressionFunctions::function_Abs(ExpressionFunction *c, ExpressionValue &result)
{
    ExpressionValue a;
    if (!c->arg(0, a))
        return false;

    result = a.isInteger() ? abs(a.asInteger()) : fabs(a.asDouble());

    return true;
}

// Return invserse cosine of argument
bool ExpressionFunctions::function_ACos(ExpressionFunction *c, ExpressionValue &result)
{
    result = (assumeDegrees_ ? acos(c->argd(0)) * DEGRAD : acos(c->argd(0)));
    return true;
}

// Return invserse sine of argument
bool ExpressionFunctions::function_ASin(ExpressionFunction *c, ExpressionValue &result)
{
    result = (assumeDegrees_ ? asin(c->argd(0)) * DEGRAD : asin(c->argd(0)));
    return true;
}

// Return invserse tangent of argument
bool ExpressionFunctions::function_ATan(ExpressionFunction *c, ExpressionValue &result)
{
    result = (assumeDegrees_ ? atan(c->argd(0)) * DEGRAD : atan(c->argd(0)));
    return true;
}

// Return cosine of argument (supplied in degrees)
bool ExpressionFunctions::function_Cos(ExpressionFunction *c, ExpressionValue &result)
{
    result = (assumeDegrees_ ? cos(c->argd(0) / DEGRAD) : cos(c->argd(0)));
    return true;
}

// Return exponential of of argument
bool ExpressionFunctions::function_Exp(ExpressionFunction *c, ExpressionValue &result)
{
    result = exp(c->argd(0));
    return true;
}

// Return natural logarithm of argument
bool ExpressionFunctions::function_Ln(ExpressionFunction *c, ExpressionValue &result)
{
    result = log(c->argd(0));
    return true;
}

// Return base-10 logarithm of argument
bool ExpressionFunctions::function_Log(ExpressionFunction *c, ExpressionValue &result)
{
    result = log10(c->argd(0));
    return true;
}

// Round real value to nearest integer
bool ExpressionFunctions::function_Nint(ExpressionFunction *c, ExpressionValue &result)
{
    result = floor(c->argd(0) + 0.5);
    return true;
}

// Return sine of argument (supplied in degrees)
bool ExpressionFunctions::function_Sin(ExpressionFunction *c, ExpressionValue &result)
{
    result = (assumeDegrees_ ? sin(c->argd(0) / DEGRAD) : sin(c->argd(0)));
    return true;
}

// Return square root of argument
bool ExpressionFunctions::function_Sqrt(ExpressionFunction *c, ExpressionValue &result)
{
    result = sqrt(c->argd(0));
    return true;
}

// Return tangent of argument (supplied in degrees)
bool ExpressionFunctions::function_Tan(ExpressionFunction *c, ExpressionValue &result)
{
    result = (assumeDegrees_ ? tan(c->argd(0) / DEGRAD) : tan(c->argd(0)));
    return true;
}

/*
 * Commands
 */

// Initialise Command Pointers
void ExpressionFunctions::initPointers()
{
    /*
     * Store pointers to all command functions
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
    pointers_[Abs] = &function_Abs;
    pointers_[ACos] = &function_ACos;
    pointers_[ASin] = &function_ASin;
    pointers_[ATan] = &function_ATan;
    pointers_[Cos] = &function_Cos;
    pointers_[Exp] = &function_Exp;
    pointers_[Ln] = &function_Ln;
    pointers_[Log] = &function_Log;
    pointers_[Nint] = &function_Nint;
    pointers_[Sin] = &function_Sin;
    pointers_[Sqrt] = &function_Sqrt;
    pointers_[Tan] = &function_Tan;
}

// Execute command
bool ExpressionFunctions::call(ExpressionFunctions::Function cf, ExpressionFunction *node, ExpressionValue &result)
{
    return (this->pointers_[cf])(node, result);
}
