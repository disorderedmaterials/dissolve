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

#include "math/broadeningfunction.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
BroadeningFunction::BroadeningFunction()
{
	function_ = BroadeningFunction::UnityFunction;
	for (int n=0; n<MAXFUNCTIONPARAMS; ++n) parameters_[n] = 0.0;
	inverted_ = false;
}

// Assignment operator
void BroadeningFunction::operator=(const BroadeningFunction& source)
{
	function_ = source.function_;
	for (int n=0; n<MAXFUNCTIONPARAMS; ++n) parameters_[n] = source.parameters_[n];
	inverted_ = source.inverted_;
}

// Destructor
BroadeningFunction::~BroadeningFunction()
{
}

const char* BroadeningFunctionKeywords[] = { "Unity", "Gaussian", "QDependentGaussian", "GaussianC2" };
int BroadeningFunctionNParameters[] = { 0, 1, 1, 2 };

// Return FunctionType from supplied string
BroadeningFunction::FunctionType BroadeningFunction::functionType(const char* s)
{
	for (int n=0; n<nFunctionTypes; ++n) if (DUQSys::sameString(s, BroadeningFunctionKeywords[n])) return (FunctionType) n;
	return BroadeningFunction::nFunctionTypes;
}

// Return FunctionType name
const char* BroadeningFunction::functionType(BroadeningFunction::FunctionType func)
{
	return BroadeningFunctionKeywords[func];
}

// Return number of parameters needed to define function
int BroadeningFunction::nFunctionParameters(FunctionType func)
{
	return BroadeningFunctionNParameters[func];
}

// Return description for FunctionType
const char* BroadeningFunction::functionDescription(FunctionType func)
{
	switch (func)
	{
		case (BroadeningFunction::UnityFunction):
			return "Function that always returns 1.0";
			break;
		case (BroadeningFunction::GaussianFunction):
			return "Gaussian (no prefactor, unnormalised)";
			break;
		case (BroadeningFunction::QDependentGaussianFunction):
			return "Gaussian (no prefactor, unnormalised, omega dependent FWHM)";
			break;
		case (BroadeningFunction::GaussianC2Function):
			return "Gaussian (no prefactor, unnormalised, independent and omega-dependent FWHM, EPSR-style)";
			break;
		default:
			break;
	}

	return "NO BROADENING FUNCTION DESCRIPTION AVAILABLE.";
}

/*
 * Function Data
 */

void BroadeningFunction::set(BroadeningFunction::FunctionType function, double p1, double p2, double p3, double p4, double p5, double p6)
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
bool BroadeningFunction::set(LineParser& parser, int startArg)
{
	// First argument is the form of the function
	BroadeningFunction::FunctionType funcType = BroadeningFunction::functionType(parser.argc(startArg));
	if (funcType == BroadeningFunction::nFunctionTypes)
	{
		Messenger::error("Unrecognised Function type '%s'.\n", parser.argc(startArg));
		return false;
	}

	// Do we have the right number of arguments for the function specified?
	if ((parser.nArgs() - startArg) < BroadeningFunction::nFunctionParameters(funcType))
	{
		Messenger::error("Too few parameters supplied for Function '%s' (expected %i, found %i).\n", BroadeningFunction::functionType(funcType), BroadeningFunction::nFunctionParameters(funcType), parser.nArgs() - startArg);
		return false;
	}

	// Set up function
	function_ = funcType;
	switch (function_)
	{
		case (BroadeningFunction::UnityFunction):
			break;
		case (BroadeningFunction::GaussianFunction):
		case (BroadeningFunction::QDependentGaussianFunction):
			// FWHM
			parameters_[0] = parser.argd(startArg+1);
			// c (calculated from FWHM)
			parameters_[1] = parameters_[0] / (2.0 * sqrt(2.0 * log(2.0)));
			// 1/c
			parameters_[2] = 1.0 / parameters_[1];
			break;
		case (BroadeningFunction::GaussianC2Function):
			// FWHM1
			parameters_[0] = parser.argd(startArg+1);
			// FWHM2
			parameters_[1] = parser.argd(startArg+2);
			// c1 (calculated from FWHM1)
			parameters_[2] = parameters_[0] / (2.0 * sqrt(2.0 * log(2.0)));
			// c2 (calculated from FWHM2)
			parameters_[3] = parameters_[1] / (2.0 * sqrt(2.0 * log(2.0)));
			// 1/c1
			parameters_[4] = 1.0 / parameters_[2];
			// 1/c2
			parameters_[5] = 1.0 / parameters_[3];
			break;
		default:
			Messenger::error("Function form '%s' not accounted for in set(LineParser&,int).\n", BroadeningFunction::functionType(funcType));
			return false;
	}

	return true;
}

// Return function type
BroadeningFunction::FunctionType BroadeningFunction::function() const
{
	return function_;
}

// Return short summary of function parameters
CharString BroadeningFunction::parameterSummary() const
{
	switch (function_)
	{
		case (BroadeningFunction::UnityFunction):
			return "1.0";
			break;
		case (BroadeningFunction::GaussianFunction):
		case (BroadeningFunction::QDependentGaussianFunction):
			return CharString("FWHM=%f", parameters_[0]);
			break;
		case (BroadeningFunction::GaussianC2Function):
			return CharString("FWHM1=%f, FWHM2=%f", parameters_[0], parameters_[1]);
			break;
		default:
			Messenger::warn("BroadeningFunction::value(x) - Function id %i not accounted for.\n", function_);
			break;
	}

	return "NULL";
}

// Set inversion state
void BroadeningFunction::setInverted(bool state)
{
	inverted_ = state;
}

// Return value of function given parameters x and Q
double BroadeningFunction::y(double x, double Q) const
{
	return (inverted_ ? yFTActual(x, Q) : yActual(x, Q));
}

// Return value of Fourier transform of function, given parameters x and Q
double BroadeningFunction::yFT(double x, double Q) const
{
	return (inverted_ ? yActual(x, Q) : yFTActual(x, Q));
}

// Return value of function given parameters x and Q, regardless of inversion state
double BroadeningFunction::yActual(double x, double Q) const
{
	switch (function_)
	{
		case (BroadeningFunction::UnityFunction):
			return 1.0;
			break;
		case (BroadeningFunction::GaussianFunction):
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
			return exp(-(0.5 * x*x * parameters_[2]*parameters_[2]));
			break;
		case (BroadeningFunction::QDependentGaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
			 * 
			 * Parameters:  0 = FWHM
			 * 		1 = c     	(precalculated from FWHM)
			 * 		2 = 1.0 / c
			 * 
			 * 	      (       x * x     )		     FWHM
			 * f(x) = exp ( - ------------- )      where c = --------------
			 * 	      (   2 * (c*Q)**2 )		 2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5 * x*x * (parameters_[2]*Q) * (parameters_[2]*Q)));
			break;
		case (BroadeningFunction::GaussianC2Function):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
			 * 
			 * Parameters:  0 = FWHM1
			 * 		1 = FWHM2
			 * 		2 = c1     	(precalculated from FWHM1)
			 * 		3 = c2     	(precalculated from FWHM2)
			 * 		4 = 1.0 / c1
			 * 		5 = 1.0 / c2
			 * 
			 * 	      (         a1 * a1       )			   FWHMn
			 * f(x) = exp ( - ------------------- )      where cn = --------------
			 * 	      (   2 * (c1 + c2*a2)**2 )		       2 sqrt(2 ln 2) 
			 */
			return exp(-(x*x)/(2.0 * (parameters_[2] + parameters_[3]*Q) * (parameters_[2] + parameters_[3]*Q)));
			break;
		default:
			Messenger::warn("BroadeningFunction::value() - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return value of Fourier transform of function, given parameters x and Q, regardless of inversion state
double BroadeningFunction::yFTActual(double x, double Q) const
{
	switch (function_)
	{
		case (BroadeningFunction::UnityFunction):
			return 1.0;
			break;
		case (BroadeningFunction::GaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero
			 * 
			 * Parameters:  0 = FWHM
			 * 		1 = c     	(precalculated from FWHM)
			 * 		2 = 1.0 / c
			 * 
			 * 	      (   a * a * c * c ) 		      FWHM
			 * f(x) = exp ( - ------------- )      where c = --------------
			 * 	      (         2       )	         2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5 * x*x * parameters_[1]*parameters_[1]));
			break;
		case (BroadeningFunction::QDependentGaussianFunction):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
			 * 
			 * Parameters:  0 = FWHM1
			 * 		1 = c     	(precalculated from FWHM1)
			 * 		2 = 1.0 / c
			 * 
			 * 	      (   x*x * (c*Q)**2 ) 		      FWHM
			 * f(x) = exp ( - -------------- )      where c = --------------
			 * 	      (         2        )	          2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5 * x*x * (parameters_[1]*Q) * (parameters_[1]*Q)));
			break;
		case (BroadeningFunction::GaussianC2Function):
			/*
			 * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
			 * 
			 * Parameters:  0 = FWHM1
			 * 		1 = FWHM2
			 * 		2 = c1     	(precalculated from FWHM1)
			 * 		3 = c2     	(precalculated from FWHM2)
			 * 		4 = 1.0 / c1
			 * 		5 = 1.0 / c2
			 * 
			 * 	      (   x * x * (c1 + c2*Q)**2 ) 		       FWHMn
			 * f(x) = exp ( - ---------------------- )      where cn = --------------
			 * 	      (             2            )	           2 sqrt(2 ln 2) 
			 */
			return exp(-(0.5 * x*x * (parameters_[2] + parameters_[3]*Q) * (parameters_[2] + parameters_[3]*Q)));
			break;
		default:
			Messenger::warn("BroadeningFunction::ft() - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

// Return unity function
BroadeningFunction& BroadeningFunction::unity()
{
	static BroadeningFunction unity;
	return unity;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool BroadeningFunction::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	if (!procPool.broadcast(EnumCast<BroadeningFunction::FunctionType>(function_), root)) return false;
	if (!procPool.broadcast(parameters_, MAXFUNCTIONPARAMS, root)) return false;
#endif
	return true;
}
