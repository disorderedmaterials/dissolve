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
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

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
	return Function::nFunctionTypes;
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

// Set function data from LineParser source
bool Function::set(LineParser& parser, int startArg)
{
	// First argument is the form of the function
	Function::FunctionType funcType = Function::functionType(parser.argc(startArg));
	if (funcType == Function::nFunctionTypes)
	{
		Messenger::error("Unrecognised Function type '%s'.\n", parser.argc(startArg));
		return false;
	}

	// Do we have the right number of arguments for the function specified?
	if ((parser.nArgs() - startArg) < Function::nFunctionParameters(funcType))
	{
		Messenger::error("Too few parameters supplied for Function '%s' (expected %i, found %i).\n", Function::functionType(funcType), Function::nFunctionParameters(funcType), parser.nArgs() - startArg);
		return false;
	}

	// Set up function
	switch (funcType)
	{
		case (Function::UnityFunction):
			break;
		default:
			Messenger::error("Function form '%s' not accounted for in set(LineParser&,int).\n", Function::functionType(funcType));
			return false;
	}

	return true;
}

// Return function type
Function::FunctionType Function::function() const
{
	return function_;
}

// Return short summary of function parameters
CharString Function::summary() const
{
	switch (function_)
	{
		case (Function::UnityFunction):
			return "1.0";
			break;
		default:
			Messenger::warn("Function::value(x) - Function id %i not accounted for.\n", function_);
			break;
	}

	return "NULL";
}

// Return value of function at value x
double Function::value(double x) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 2))
	{
		Messenger::error("Function::value(x) - Function '%s' requires %i arguments, not 1.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif

	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		default:
			Messenger::warn("Function::value(x) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of function at values x and y
double Function::value(double x, double y) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 2))
	{
		Messenger::error("Function::value(x,y) - Function '%s' requires %i arguments, not 2.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif
	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		default:
			Messenger::warn("Function::value(x,y) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of function at values x, y, and z
double Function::value(double x, double y, double z) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 2))
	{
		Messenger::error("Function::value(x,y) - Function '%s' requires %i arguments, not 3.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif

	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		default:
			Messenger::warn("Function::value(x,y,z) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return unity function
Function& Function::unity()
{
	static Function unity;
	return unity;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Function::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	if (!procPool.broadcast(EnumCast<Function::FunctionType>(function_), root)) return false;
	if (!procPool.broadcast(parameters_, MAXFUNCTIONPARAMS, root)) return false;
#endif
	return true;
}
