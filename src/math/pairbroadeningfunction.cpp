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
#include "data/atomicmass.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "templates/enumhelpers.h"
#include "templates/genericitemcontainer_array2ddouble.h"

// Constructor
PairBroadeningFunction::PairBroadeningFunction(PairBroadeningFunction::FunctionType function)
{
	function_ = function;

	// Create element broadening array
	elementPairGaussianFWHM_.initialise(Elements::nElements(), Elements::nElements(), true);
	elementPairGaussianFlags_.initialise(Elements::nElements(), Elements::nElements(), true);
	elementPairGaussianFWHM_ = 0.13;
	elementPairGaussianFlags_ = false;
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

	gaussianFWHM_ = source.gaussianFWHM_;
	
	elementPairGaussianFWHM_ = source.elementPairGaussianFWHM_;
	elementPairGaussianFlags_ = source.elementPairGaussianFlags_;
}

const char* PairBroadeningFunctionKeywords[] = { "None", "Gaussian", "GaussianElements" };
int PairBroadeningFunctionNParameters[] = { 0, 1, 0 };

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

/*
 * Function Data
 */

// Read function data from LineParser source
bool PairBroadeningFunction::readAsKeyword(LineParser& parser, int startArg)
{
	// First argument is the form of the function, or a '&' to indicate that a full block-style definition of the data
	if (DissolveSys::sameString("&", parser.argc(startArg))) return read(parser);

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
			gaussianFWHM_ = parser.argd(startArg+1);
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			Messenger::print("Gaussian element-pair broadening requested - default starting values will be used.\n");
			break;
		default:
			Messenger::error("Function form '%s' not accounted for in PairBroadeningFunction::set(LineParser&,int).\n", PairBroadeningFunction::functionType(funcType));
			return false;
	}

	// Note: Setting up dependent parameters cannot be done here, as we do not know the AtomTypes involved in the pair

	return true;
}

// Write function data to LineParser source
bool PairBroadeningFunction::writeAsKeyword(LineParser& parser, const char* prefix)
{
	// Write function name as first line
	if (!parser.writeLineF("%s%s\n", prefix, functionType(function_))) return false;

	// Write any additional info here
	int count = 0;
	switch (function_)
	{
		case (PairBroadeningFunction::GaussianFunction):
			if (!parser.writeLineF("%s%e\n", prefix, gaussianFWHM_)) return false;
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			// Count number of pairs/values to expect and write to file
			for (int n = 0; n<elementPairGaussianFlags_.linearArraySize(); ++n) if (elementPairGaussianFlags_.constLinearValue(n)) ++count;
			if (!parser.writeLineF("%s%i\n", prefix, count)) return false;

			// Loop again and write the data proper
			for (int i = 0; i < elementPairGaussianFlags_.nRows(); ++i)
			{
				for (int j = 0; j < elementPairGaussianFlags_.nColumns(); ++j)
				{
					if (elementPairGaussianFlags_.constAt(i, j))
					{
						if (!parser.writeLineF("%s%s  %s  %f\n", prefix, Elements::element(i).symbol(), Elements::element(j).symbol(), elementPairGaussianFWHM_.constAt(i,j))) return false;
					}
				}
			}
		default:
			break;
	}

	return true;
}

void PairBroadeningFunction::setFunction(PairBroadeningFunction::FunctionType function)
{
	function_ = function;
}

// Return function type
PairBroadeningFunction::FunctionType PairBroadeningFunction::function() const
{
	return function_;
}

// Set Gaussian FWHM parameters
void PairBroadeningFunction::setGaussianFWHM(double fwhm)
{
	gaussianFWHM_ = fwhm;
}

// Return Gaussian FWHM parameter
double PairBroadeningFunction::gaussianFWHM() const
{
	return gaussianFWHM_;
}

// Return array of pointers to all adjustable parameters
Array<double*> PairBroadeningFunction::parameters()
{
	Array<double*> params;

	switch (function_)
	{
		case (PairBroadeningFunction::NoFunction):
			break;
		case (PairBroadeningFunction::GaussianFunction):
			params.add(&gaussianFWHM_);
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			for (int n = 0; n<elementPairGaussianFlags_.linearArraySize(); ++n)
			{
				if (elementPairGaussianFlags_.constLinearValue(n)) params.add(&elementPairGaussianFWHM_.linearValue(n));
			}
			break;
		default:
			break;
	}

	return params;
}

// Return short summary of function parameters
CharString PairBroadeningFunction::summary() const
{
	CharString result = "???";

	switch (function_)
	{
		case (PairBroadeningFunction::NoFunction):
			result = "None";
			break;
		case (PairBroadeningFunction::GaussianFunction):
			result.sprintf("Gaussian, FWHM=%f", gaussianFWHM_);
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			result = "Gaussian [Z1-Z2]";
			break;
		default:
			break;
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
			// Simple broadening - same FWHM for any AtomType pair
			result.set(BroadeningFunction::GaussianFunction, gaussianFWHM_);
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			// If this matrix value has never been used/read, set the flag now
			if (!elementPairGaussianFlags_.at(at1->element()->Z(), at2->element()->Z())) elementPairGaussianFlags_.at(at1->element()->Z(), at2->element()->Z()) = true;
			result.set(BroadeningFunction::GaussianFunction, elementPairGaussianFWHM_.at(at1->element()->Z(), at2->element()->Z()));
			break;
			// POSSIBLE USE AS FUNCTION FOR ELEMENT/ATOMTYPE-DEPENDENT BROADENING?
// 		case (PairBroadeningFunction::GaussianElementFunction):
// 			// Calculate reduced mass (store in parameters_[1])
// 			parameters_[1] = sqrt((AtomicMass::mass(at1->element()) * AtomicMass::mass(at2->element())) / (AtomicMass::mass(at1->element()) + AtomicMass::mass(at2->element())));
// 
// 			// Calculate final broadening
// 			parameters_[0] = 1.0 / (2.0 * sqrt(2.0) * parameters_[1]);
// 
// 			result.set(BroadeningFunction::GaussianFunction, parameters_[0]);
			break;
		default:
			Messenger::error("Function form '%s' not accounted for in setUpDependentParameters().\n", PairBroadeningFunction::functionType(function_));
	}

	return result;
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
	return writeAsKeyword(parser, "");
}

// Read data through specified LineParser
bool PairBroadeningFunction::read(LineParser& parser)
{
	// First line is function name
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	function_ = functionType(parser.argc(0));
	if (function_ == nFunctionTypes) return Messenger::error("Unrecognised pair broadening function '%s'.\n", parser.argc(0));

	// Our next action depends on the function
	switch (function_)
	{
		case (PairBroadeningFunction::GaussianFunction):
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			gaussianFWHM_ = parser.argd(0);
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			// First line is the number of values we need to read
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			else
			{
				// Reset the parameters to boring default values
				elementPairGaussianFWHM_ = 0.12;
				elementPairGaussianFlags_ = false;

				int nPairs = parser.argi(0);
				for (int n=0; n<nPairs; ++n)
				{
					if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

					// Line format is:  Element1  Element2  FWHM
					Element& el1 = Elements::element(parser.argc(0));
					if (el1.isUnknown()) return Messenger::error("Unrecognised element '%s' found in pair broadening parameters.\n", parser.argc(0));
					Element& el2 = Elements::element(parser.argc(1));
					if (el2.isUnknown()) return Messenger::error("Unrecognised element '%s' found in pair broadening parameters.\n", parser.argc(1));

					// Set the value
					elementPairGaussianFlags_.at(el1.Z(), el2.Z()) = parser.argd(2);
					elementPairGaussianFlags_.at(el1.Z(), el2.Z()) = true;
				}
			}
			break;
		default:
			break;
	}

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
	if (!procPool.broadcast(gaussianFWHM_, root)) return false;
	if (!procPool.broadcast(elementPairGaussianFWHM_, root)) return false;
	if (!procPool.broadcast(elementPairGaussianFlags_, root)) return false;
#endif
	return true;
}

// Check item equality
bool PairBroadeningFunction::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(EnumCast<PairBroadeningFunction::FunctionType>(function_))) return Messenger::error("PairBroadeningFunction function type is not equivalent (process %i has %i).\n", procPool.poolRank(), function_);
	if (!procPool.equality(gaussianFWHM_)) return Messenger::error("PairBroadeningFunction Gaussian parameters are not equivalent.\n");
	if (!procPool.equality(elementPairGaussianFWHM_)) return Messenger::error("PairBroadeningFunction element pair Gaussian parameters are not equivalent.\n");
	if (!procPool.equality(elementPairGaussianFlags_)) return Messenger::error("PairBroadeningFunction element pair Gaussian parameters are not equivalent.\n");
#endif
	return true;
}
