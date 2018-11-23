/*
	*** Expression Functions
	*** src/expression/functions.h
	Copyright T. Youngs 2015-2018

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

#ifndef DISSOLVE_FUNCTIONS_H
#define DISSOLVE_FUNCTIONS_H

// Forward declarations
class FunctionNode;

// Function pointer typedef and call #define
typedef bool (*FunctionFunction)(FunctionNode* c, double& rv);
#define CALL_COMMAND(object,ptrToMember) ((object).*(ptrToMember)) 

// Function data 
class FunctionData
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

// Function actions
class Functions
{
	public:
	// Constructor / Destructor
	Functions();
	~Functions();


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
	static Functions::Function function(const char* s);


	/*
	 * Function declarations
	 */
	private:
	// AST-specific functions
	static bool function_NoFunction(FunctionNode* c, double& rv);
	static bool function_Joiner(FunctionNode* c, double& rv);
	// Flow control
	static bool function_If(FunctionNode* c, double& rv);
	// Math Functions
	static bool function_Abs(FunctionNode* c, double& rv);
	static bool function_ACos(FunctionNode* c, double& rv);
	static bool function_ASin(FunctionNode* c, double& rv);
	static bool function_ATan(FunctionNode* c, double& rv);
	static bool function_Cos(FunctionNode* c, double& rv);
	static bool function_Exp(FunctionNode* c, double& rv);
	static bool function_Ln(FunctionNode* c, double& rv);
	static bool function_Log(FunctionNode* c, double& rv);
	static bool function_Nint(FunctionNode* c, double& rv);
	static bool function_Sin(FunctionNode* c, double& rv);
	static bool function_Sqrt(FunctionNode* c, double& rv);
	static bool function_Tan(FunctionNode* c, double& rv);
	// Variable Operators
	static bool function_OperatorAdd(FunctionNode* c, double& rv);
	static bool function_OperatorAnd(FunctionNode* c, double& rv);
	static bool function_OperatorDivide(FunctionNode* c, double& rv);
	static bool function_OperatorEqualTo(FunctionNode* c, double& rv);
	static bool function_OperatorGreaterThan(FunctionNode* c, double& rv);
	static bool function_OperatorGreaterThanEqualTo(FunctionNode* c, double& rv);
	static bool function_OperatorLessThan(FunctionNode* c, double& rv);
	static bool function_OperatorLessThanEqualTo(FunctionNode* c, double& rv);
	static bool function_OperatorModulus(FunctionNode* c, double& rv);
	static bool function_OperatorMultiply(FunctionNode* c, double& rv);
	static bool function_OperatorNegate(FunctionNode* c, double& rv);
	static bool function_OperatorNot(FunctionNode* c, double& rv);
	static bool function_OperatorNotEqualTo(FunctionNode* c, double& rv);
	static bool function_OperatorOr(FunctionNode* c, double& rv);
	static bool function_OperatorPower(FunctionNode* c, double& rv);
	static bool function_OperatorSubtract(FunctionNode* c, double& rv);
	// Other Functions
	static bool function_PrintF(FunctionNode* c, double& rv);
	
	/*
	 * Function descriptions / syntax etc.
	 */
	private:
	// Function pointers
	FunctionFunction pointers_[Functions::nFunctions];

	public:
	// Function data
	static FunctionData data[Functions::nFunctions];
	// Return specified function keyword
	static const char* keyword(Functions::Function func);
	// Return specified function arguments
	static const char* arguments(Functions::Function func);
	// Return whether specified function returns a number
	static bool returnsNumber(Functions::Function func);
	// Return specified function argument names
	static const char* argText(Functions::Function func);
	// Return specified function syntax
	static const char* syntax(Functions::Function func);
	// Initialise function pointers
	void initPointers();
	// Execute specified function
	bool call(Functions::Function cf, FunctionNode* node, double& rv);
};

// External Singleton
extern Functions functions;

#endif

