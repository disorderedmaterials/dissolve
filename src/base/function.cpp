/*
	*** Function
	*** src/base/function.cpp
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

#include "base/function.h"
#include "base/processpool.h"
#include "base/sysfunc.h"

// Constructor
Function::Function()
{
	function_ = Function::UnityFunction;
	for (int n=0; n<MAXFUNCTIONPARAMS; ++n) parameters_[n] = 0.0;
}

// Assignment operator
void Function::operator=(const Function& source)
{
	function_ = source.function_;
	for (int n=0; n<MAXFUNCTIONPARAMS; ++n) parameters_[n] = source.parameters_[n];
}

// Destructor
Function::~Function()
{
}

/*
 * Function Definitions
 */

FunctionDefinition Function::functions_[] = {
	{ "Unity",		0,	{ "_NO PARAMS_" },
				1,	{ "x" },
				"Function that always returns 1.0." }
};

// Return FunctionType from supplied string
Function::FunctionType Function::functionType(const char* s)
{
	for (int n=0; n<nFunctionTypes; ++n) if (DUQSys::sameString(s, functions_[n].name)) return (FunctionType) n;
	return Function::UnityFunction;
}

// Return FunctionType name
const char* Function::functionType(Function::FunctionType func)
{
	return functions_[func].name;
}

// Return number of parameters needed to define function
int Function::nFunctionParameters(FunctionType func)
{
	return functions_[func].nParameters;
}

/*
 * Function Data
 */

void Function::set(FunctionType function, double a, double b, double c, double d, double e, double f)
{
	function_ = function;
	parameters_[0] = a;
	parameters_[1] = b;
	parameters_[2] = c;
	parameters_[3] = d;
	parameters_[4] = e;
	parameters_[5] = f;
}

// Return value of function at value x
double Function::value(double x)
{
}

// Return value of function at values x and y
double Function::value(double x, double y)
{
}

// Return value of function at values x, y, and z
double Function::value(double x, double y, double z)
{
}
