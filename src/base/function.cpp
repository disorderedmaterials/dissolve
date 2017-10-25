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
				"Function that always returns 1.0." },
	{ "Gaussian",		1,	{ "FWHM" },
				1,	{ "x" },
				"Gaussian (no prefactor, unnormalised)" },
	{ "DependentGaussian",	1,	{ "FWHM" },
				2,	{ "x", "x2" },
				"Gaussian (no prefactor, unnormalised, dependent FWHM)" }
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

void Function::set(Function::FunctionType function, double p1, double p2, double p3, double p4, double p5, double p6)
{
	function_ = function;
	parameters_[0] = p1;
	parameters_[1] = p2;
	parameters_[2] = p3;
	parameters_[3] = p4;
	parameters_[4] = p5;
	parameters_[5] = p6;
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
	function_ = funcType;
	switch (function_)
	{
		case (Function::UnityFunction):
			break;
		case (Function::GaussianFunction):
		case (Function::DependentGaussianFunction):
			// FWHM
			parameters_[0] = parser.argd(startArg+1);
			// c (calculated from FWHM)
			parameters_[1] = parameters_[0] / (2.0 * sqrt(2.0 * log(2.0)));
			// 1/c
			parameters_[2] = 1.0 / parameters_[1];
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
		case (Function::GaussianFunction):
		case (Function::DependentGaussianFunction):
			return CharString("FWHM=%f", parameters_[0]);
			break;
		default:
			Messenger::warn("Function::value(x) - Function id %i not accounted for.\n", function_);
			break;
	}

	return "NULL";
}

// Return number of arguments accepted by function
int Function::nArguments() const
{
	return functions_[function_].nArguments;
}

// Return value of function given parameter a
double Function::value(double a) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 1))
	{
		Messenger::error("Function::value(a) - Function '%s' requires %i arguments, not 1.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif

	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		case (Function::GaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero
			 * 
			 * Parameters:  0 = FWHM
			 * 		1 = c     	(precalculated from FWHM)
			 * 		2 = 1.0 / c
			 * 
			 * 	      (     x * x   ) 			  FWHM
			 * f(x) = exp ( - --------- )      where c = --------------
			 * 	      (   2 * c * c )		     2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5*a*a*parameters_[2]*parameters_[2]));
			break;
		default:
			Messenger::warn("Function::value(a) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of function given parameters a and b
double Function::value(double a, double b) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 2))
	{
		Messenger::error("Function::value(a,b) - Function '%s' requires %i arguments, not 2.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif
	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		case (Function::DependentGaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
			 * 
			 * Parameters:  0 = FWHM
			 * 		1 = c     	(precalculated from FWHM)
			 * 		2 = 1.0 / c
			 * 
			 * 	      (     x * x   ) 			  FWHM
			 * f(x) = exp ( - --------- )      where c = --------------
			 * 	      (   2 * c * c )		     2 sqrt(2 ln 2) 
			 */
			return exp(-(a*a)/(2.0*parameters_[1]*b*parameters_[1]*b));
			break;
		default:
			Messenger::warn("Function::value(a,b) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of function given parameters a, b, and c
double Function::value(double a, double b, double c) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 3))
	{
		Messenger::error("Function::value(a,b,c) - Function '%s' requires %i arguments, not 3.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif

	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		default:
			Messenger::warn("Function::value(a,b,c) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of Fourier transform of function, given parameter a
double Function::ft(double a) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 1))
	{
		Messenger::error("Function::value(a) - Function '%s' requires %i arguments, not 1.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif

	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		case (Function::GaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero
			 * 
			 * Parameters:  0 = FWHM
			 * 		1 = c     	(precalculated from FWHM)
			 * 		2 = 1/c
			 * 
			 * 	      (   x * x * c * c ) 		      FWHM
			 * f(x) = exp ( - ------------- )      where c = --------------
			 * 	      (         2       )	         2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5 * a*a * parameters_[1]*parameters_[1]));
			break;
		default:
			Messenger::warn("Function::value(a) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of Fourier transform of function, given parameters a and b
double Function::ft(double a, double b) const
{
#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 2))
	{
		Messenger::error("Function::ft(a,b) - Function '%s' requires %i arguments, not 2.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif
	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		case (Function::DependentGaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
			 * 
			 * Parameters:  0 = FWHM
			 * 		1 = c     	(precalculated from FWHM)
			 * 		2 = 1/c
			 * 
			 * 	      (   x * x * c * c ) 		      FWHM
			 * f(x) = exp ( - ------------- )      where c = --------------
			 * 	      (         2       )	         2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5 * a*a * parameters_[1]*b*parameters_[1]*b));
			break;
		default:
			Messenger::warn("Function::ft(a,b) - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of Fourier transform of function, given parameters a, b, and c
double Function::ft(double a, double b, double c) const
{
	#ifdef CHECKS
	if ((function_ != Function::UnityFunction) && (functions_[function_].nArguments != 2))
	{
		Messenger::error("Function::ft(a,b,c) - Function '%s' requires %i arguments, not 3.\n", Function::functionType(function_), functions_[function_].nArguments);
		return 0.0;
	}
#endif

	switch (function_)
	{
		case (Function::UnityFunction):
			return 1.0;
			break;
		default:
			Messenger::warn("Function::ft(a,b,c) - Function id %i not accounted for.\n", function_);
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
