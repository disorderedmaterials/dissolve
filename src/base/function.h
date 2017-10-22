/*
	*** Plain (old datatype) Value
	*** src/base/plainvalue.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_FUNCTION_H
#define DUQ_FUNCTION_H

#define MAXFUNCTIONPARAMS 6

// Forward Declarations
/* none */

// Function Definitions
class FunctionDefinition
{
	public:
	// Name of function
	const char* name;
	// Number of parameters needed to define function
	int nParameters;
	// Array of parameter names
	const char* parameterNames[MAXFUNCTIONPARAMS];
	// Number of arguments taken by function
	int nArguments;
	// Array of argument names
	const char* argumentNames[MAXFUNCTIONPARAMS];
	// Function description
	const char* description;
};

// Function
class Function
{
	public:
	// Constructor
	Function();
	// Destructor
	~Function();
	// Assignment Operator
	void operator=(const Function& source);
	// Function Types
	enum FunctionType { UnityFunction, nFunctionTypes };
	// Return FunctionType from supplied string
	static FunctionType functionType(const char* s);
	// Return FunctionType name
	static const char* functionType(FunctionType func);
	// Return number of parameters needed to define function
	static int nFunctionParameters(FunctionType func);


	/*
	 * Function Definitions
	 */
	private:
	// Function Data
	static FunctionDefinition functions_[];


	/*
	 * Function Data
	 */
	private:
	// Function Type
	FunctionType function_;
	// Parameters
	double parameters_[MAXFUNCTIONPARAMS];

	public:
	/* Set function data
	 * Function Type	Parameters
	 * XXX
	 */
	void set(FunctionType function, double a = 0.0, double b = 0.0, double c = 0.0, double d = 0.0, double e = 0.0, double f = 0.0);
	// Return value of function at value x
	double value(double x);
	// Return value of function at values x and y
	double value(double x, double y);
	// Return value of function at values x, y, and z
	double value(double x, double y, double z);
};

#endif

