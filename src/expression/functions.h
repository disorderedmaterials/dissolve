/*
	*** Expression Functions
	*** src/expression/functions.h
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

#ifndef DISSOLVE_EXPRESSION_FUNCTIONS_H
#define DISSOLVE_EXPRESSION_FUNCTIONS_H

#include "expression/result.h"

// Forward declarations
class ExpressionFunction;

// Function pointer typedef and call #define
typedef bool (*FunctionFunction)(ExpressionFunction* c, double& result);
#define CALL_COMMAND(object,ptrToMember) ((object).*(ptrToMember)) 

// Function data 
class ExpressionFunctionData
{
	public:
	// Function keyword
	const char* keyword;
	// Function arguments
	const char* arguments;
	// Whether function returns a number
	bool returnsNumber;
	// Function argument names
	const char* argText;
	// Function syntax
	const char* syntax;
};

// Expression Functions
class ExpressionFunctions
{
	public:
	// Constructor / Destructor
	ExpressionFunctions();
	~ExpressionFunctions();


	/*
	 * Settings
	 */
	private:
	// Whether to assume values used in trig functions are degrees, and need to be converted to radians
	static bool assumeDegrees_;

	public:
	// Set whether to assume values used in trig functions are degrees
	static void setAssumeDegrees(bool b);
	// Return whether to assume values used in trig functions are degrees
	static bool assumeDegrees();


	/*
	 * Function list
	 */
	enum Function
	{
		// Operators
		OperatorAdd,
		OperatorAnd,
		OperatorDivide,
		OperatorEqualTo,
		OperatorGreaterThan,
		OperatorGreaterThanEqualTo,
		OperatorLessThan,
		OperatorLessThanEqualTo,
		OperatorModulus,
		OperatorMultiply,
		OperatorNegate,
		OperatorNot,
		OperatorNotEqualTo,
		OperatorOr,
		OperatorPower,
		OperatorSubtract,

		// Flow control
		NoFunction,
		Joiner,
		If,

		// Math Functions
		Abs,
		ACos,
		ASin,
		ATan,
		Cos,
		Exp,
		Ln,
		Log,
		Nint,
		Sin,
		Sqrt,
		Tan,

		nFunctions
	};
	// Return enumerated function id from string
	static Function function(const char* s);


	/*
	 * Function declarations
	 */
	private:
	// AST-specific functions
	static bool function_NoFunction(ExpressionFunction* c, double& result);
	static bool function_Joiner(ExpressionFunction* c, double& result);
	// Flow control
	static bool function_If(ExpressionFunction* c, double& result);
	// Math Functions
	static bool function_Abs(ExpressionFunction* c, double& result);
	static bool function_ACos(ExpressionFunction* c, double& result);
	static bool function_ASin(ExpressionFunction* c, double& result);
	static bool function_ATan(ExpressionFunction* c, double& result);
	static bool function_Cos(ExpressionFunction* c, double& result);
	static bool function_Exp(ExpressionFunction* c, double& result);
	static bool function_Ln(ExpressionFunction* c, double& result);
	static bool function_Log(ExpressionFunction* c, double& result);
	static bool function_Nint(ExpressionFunction* c, double& result);
	static bool function_Sin(ExpressionFunction* c, double& result);
	static bool function_Sqrt(ExpressionFunction* c, double& result);
	static bool function_Tan(ExpressionFunction* c, double& result);
	// Variable Operators
	static bool function_OperatorAdd(ExpressionFunction* c, double& result);
	static bool function_OperatorAnd(ExpressionFunction* c, double& result);
	static bool function_OperatorDivide(ExpressionFunction* c, double& result);
	static bool function_OperatorEqualTo(ExpressionFunction* c, double& result);
	static bool function_OperatorGreaterThan(ExpressionFunction* c, double& result);
	static bool function_OperatorGreaterThanEqualTo(ExpressionFunction* c, double& result);
	static bool function_OperatorLessThan(ExpressionFunction* c, double& result);
	static bool function_OperatorLessThanEqualTo(ExpressionFunction* c, double& result);
	static bool function_OperatorModulus(ExpressionFunction* c, double& result);
	static bool function_OperatorMultiply(ExpressionFunction* c, double& result);
	static bool function_OperatorNegate(ExpressionFunction* c, double& result);
	static bool function_OperatorNot(ExpressionFunction* c, double& result);
	static bool function_OperatorNotEqualTo(ExpressionFunction* c, double& result);
	static bool function_OperatorOr(ExpressionFunction* c, double& result);
	static bool function_OperatorPower(ExpressionFunction* c, double& result);
	static bool function_OperatorSubtract(ExpressionFunction* c, double& result);
	// Other Functions
	static bool function_PrintF(ExpressionFunction* c, double& result);


	/*
	 * Function descriptions / syntax etc.
	 */
	private:
	// Function pointers
	FunctionFunction pointers_[ExpressionFunctions::nFunctions];

	public:
	// Function data
	static ExpressionFunctionData data[ExpressionFunctions::nFunctions];
	// Return specified function keyword
	static const char* keyword(ExpressionFunctions::Function func);
	// Return specified function arguments
	static const char* arguments(ExpressionFunctions::Function func);
	// Return whether specified function returns a number
	static bool returnsNumber(ExpressionFunctions::Function func);
	// Return specified function argument names
	static const char* argText(ExpressionFunctions::Function func);
	// Return specified function syntax
	static const char* syntax(ExpressionFunctions::Function func);
	// Initialise function pointers
	void initPointers();
	// Execute specified function
	bool call(ExpressionFunctions::Function cf, ExpressionFunction* node, double& result);
};

// External Singleton
extern ExpressionFunctions expressionFunctions;

#endif

