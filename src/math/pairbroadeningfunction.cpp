/*
	*** PairBroadening Function
	*** src/math/pairbroadeningfunction.cpp
	Copyright T. Youngs 2012-2018

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

#include "math/pairbroadeningfunction.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
PairBroadeningFunction::PairBroadeningFunction(PairBroadeningFunction::FunctionType function, double p1, double p2, double p3, double p4, double p5, double p6)
{
	set(function, p1, p2, p3, p4, p5, p6);
}

// Destructor
PairBroadeningFunction::~PairBroadeningFunction()
{
}

// Copy Constructor
PairBroadeningFunction::PairBroadeningFunction(const PairBroadeningFunction& source)
{
	(*this) = source;
}

// Assignment operator
void PairBroadeningFunction::operator=(const PairBroadeningFunction& source)
{
	function_ = source.function_;
	for (int n=0; n<MAXPAIRBROADENINGPARAMS; ++n) parameters_[n] = source.parameters_[n];
}

const char* PairBroadeningFunctionKeywords[] = { "None", "Gaussian", "Youngs2018v1" };
int PairBroadeningFunctionNParameters[] = { 0, 1, 0 };

const char* PairBroadeningFunctionParameters[][MAXPAIRBROADENINGPARAMS] = {
	{ "", "", "", "", "", "",},
	{ "FWHM", "", "", "", "", "",},
	{ "", "", "", "", "", "",}
};

// Return FunctionType from supplied string
PairBroadeningFunction::FunctionType PairBroadeningFunction::functionType(const char* s)
{
	for (int n=0; n<nFunctionTypes; ++n) if (DissolveSys::sameString(s, PairBroadeningFunctionKeywords[n])) return (FunctionType) n;
	return PairBroadeningFunction::nFunctionTypes;
}

// Return FunctionType name
const char* PairBroadeningFunction::functionType(PairBroadeningFunction::FunctionType func)
{
	return PairBroadeningFunctionKeywords[func];
}

// Return number of parameters needed to define function
int PairBroadeningFunction::nFunctionParameters(FunctionType func)
{
	return PairBroadeningFunctionNParameters[func];
}

// Return description for FunctionType
const char* PairBroadeningFunction::functionDescription(FunctionType func)
{
	switch (func)
	{
		case (PairBroadeningFunction::NoFunction):
			return "No Broadening";
			break;
		case (PairBroadeningFunction::GaussianFunction):
			return "Gaussian-shape Broadening)";
			break;
		case (PairBroadeningFunction::Youngs2018v1):
			return "Something made up";
			break;
		default:
			break;
	}

	return "NO BROADENING FUNCTION DESCRIPTION AVAILABLE.";
}

/*
 * Function Data
 */

void PairBroadeningFunction::set(PairBroadeningFunction::FunctionType function, double p1, double p2, double p3, double p4, double p5, double p6)
{
	function_ = function;
	parameters_[0] = p1;
	parameters_[1] = p2;
	parameters_[2] = p3;
	parameters_[3] = p4;
	parameters_[4] = p5;
	parameters_[5] = p6;

	// Setting up dependent parameters cannot be done here, as we do not know the AtomTypes involved in the pair
}

// Set function data from LineParser source
bool PairBroadeningFunction::set(LineParser& parser, int startArg)
{
	// Zero all parameters before we start
	for (int n=0; n<MAXPAIRBROADENINGPARAMS; ++n) parameters_[n] = 0.0;

	// First argument is the form of the function
	PairBroadeningFunction::FunctionType funcType = PairBroadeningFunction::functionType(parser.argc(startArg));
	if (funcType == PairBroadeningFunction::nFunctionTypes)
	{
		Messenger::error("Unrecognised Function type '%s'.\n", parser.argc(startArg));
		return false;
	}

	// Do we have the right number of arguments for the function specified?
	if ((parser.nArgs() - startArg) < PairBroadeningFunction::nFunctionParameters(funcType))
	{
		Messenger::error("Too few parameters supplied for Function '%s' (expected %i, found %i).\n", PairBroadeningFunction::functionType(funcType), PairBroadeningFunction::nFunctionParameters(funcType), parser.nArgs() - startArg);
		return false;
	}

	// Set up function and basic parameters
	function_ = funcType;
	switch (function_)
	{
		case (PairBroadeningFunction::NoFunction):
			break;
		case (PairBroadeningFunction::GaussianFunction):
			// FWHM
			parameters_[0] = parser.argd(startArg+1);
			break;
		case (PairBroadeningFunction::Youngs2018v1):
			break;
		default:
			Messenger::error("Function form '%s' not accounted for in PairBroadeningFunction::set(LineParser&,int).\n", PairBroadeningFunction::functionType(funcType));
			return false;
	}

	// Setting up dependent parameters cannot be done here, as we do not know the AtomTypes involved in the pair

	return true;
}

// Return function type
PairBroadeningFunction::FunctionType PairBroadeningFunction::function() const
{
	return function_;
}

// Return number of parameters required
int PairBroadeningFunction::nParameters() const
{
	return nFunctionParameters(function_);
}

// Return specified parameter
double PairBroadeningFunction::parameter(int index) const
{
	return parameters_[index];
}

// Return parameters array
double* PairBroadeningFunction::parameters()
{
	return parameters_;
}

// Return specified parameter name
const char* PairBroadeningFunction::parameterName(int index) const
{
	return PairBroadeningFunctionParameters[function_][index];
}

// Return short summary of function parameters
CharString PairBroadeningFunction::parameterSummary() const
{
	if (PairBroadeningFunctionNParameters[function_] == 0) return "<No Parameters>";

	CharString result;
	for (int n=0; n<PairBroadeningFunctionNParameters[function_]; ++n)
	{
		if (n == 0) result.strcatf("%s=%f", PairBroadeningFunctionParameters[function_][n], parameters_[n]);
		else result.strcatf(", %s=%f", PairBroadeningFunctionParameters[function_][n], parameters_[n]);
	}

	return result;
}

// Return a BroadeningFunction tailored to the specified AtomType pair
BroadeningFunction PairBroadeningFunction::broadeningFunction(AtomType* at1, AtomType* at2)
{
	BroadeningFunction result;

	switch (function_)
	{
		case (PairBroadeningFunction::NoFunction):
			break;
		case (PairBroadeningFunction::GaussianFunction):
			/*
			 * Set up a Gaussian function, using our stored parameter_[0] for the FWHM
			 * The FWHM is invariant with AtomType.
			 */
			result.set(BroadeningFunction::GaussianFunction, parameters_[0]);
			break;
		case (PairBroadeningFunction::Youngs2018v1):
			/*
			 * Set up a Gaussian function, with FWHM depending on the AtomTypes provided.
			 */
			parameters_[0] = 0.20;

			// c (calculated from FWHM)
			parameters_[1] = parameters_[0] / TWOSQRT2LN2;
			// 1/c
			parameters_[2] = 1.0 / parameters_[1];
			break;
			break;
		default:
			Messenger::error("Function form '%s' not accounted for in setUpDependentParameters().\n", PairBroadeningFunction::functionType(function_));
	}

	return result;
}

// Return value of function given parameters x and omega
double PairBroadeningFunction::y(double x, double omega) const
{
	switch (function_)
	{
		case (PairBroadeningFunction::NoFunction):
			return 1.0;
			break;
		case (PairBroadeningFunction::GaussianFunction):
		case (PairBroadeningFunction::Youngs2018v1):
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
		default:
			Messenger::warn("PairBroadeningFunction::value() - Function id %i not accounted for.\n", function_);
			break;
	}

	return 0.0;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* PairBroadeningFunction::itemClassName()
{
	return "PairBroadeningFunction";
}

// Write data through specified LineParser
bool PairBroadeningFunction::write(LineParser& parser)
{
	CharString line("%s", functionType(function_));
	for (int n=0; n<nFunctionParameters(function_); ++n) line.strcatf(" %16.9e", parameters_[n]);
	return parser.writeLine(line.get());
}

// Read data through specified LineParser
bool PairBroadeningFunction::read(LineParser& parser)
{
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	function_ = functionType(parser.argc(0));
	for (int n=0; n<nFunctionParameters(function_); ++n) parameters_[n] = parser.argd(n+1);
	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool PairBroadeningFunction::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	if (!procPool.broadcast(EnumCast<PairBroadeningFunction::FunctionType>(function_), root)) return false;
	if (!procPool.broadcast(parameters_, MAXPAIRBROADENINGPARAMS, root)) return false;
#endif
	return true;
}

// Check item equality
bool PairBroadeningFunction::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(EnumCast<PairBroadeningFunction::FunctionType>(function_))) return Messenger::error("PairBroadeningFunction function type is not equivalent (process %i has %i).\n", procPool.poolRank(), function_);
	if (!procPool.equality(parameters_, MAXPAIRBROADENINGPARAMS)) return Messenger::error("PairBroadeningFunction parameters are not equivalent.\n");
#endif
	return true;
}
