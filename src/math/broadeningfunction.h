/*
	*** Broadening Function
	*** src/math/broadeningfunction.h
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

#ifndef DISSOLVE_BROADENINGFUNCTION_H
#define DISSOLVE_BROADENINGFUNCTION_H

#define MAXBROADENINGFUNCTIONPARAMS 6

#include "base/charstring.h"
#include "base/genericitembase.h"

// Forward Declarations
class LineParser;
class ProcessPool;

// Broadening Function
class BroadeningFunction : public GenericItemBase
{
	public:
	// Function Types
	enum FunctionType { NoFunction, GaussianFunction, ScaledGaussianFunction, OmegaDependentGaussianFunction, GaussianC2Function, nFunctionTypes };
	// Return FunctionType from supplied string
	static FunctionType functionType(const char* s);
	// Return FunctionType name
	static const char* functionType(FunctionType func);
	// Return number of parameters needed to define FunctionType
	static int nFunctionParameters(FunctionType func);
	// Return description for FunctionType
	static const char* functionDescription(FunctionType func);

	public:
	// Constructor
	BroadeningFunction(FunctionType function = NoFunction, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0, double p6 = 0.0);
	// Destructor
	~BroadeningFunction();
	// Copy Constructor
	BroadeningFunction(const BroadeningFunction& source);
	// Assignment Operator
	void operator=(const BroadeningFunction& source);


	/*
	 * Function Data
	 */
	private:
	// Function Type
	FunctionType function_;
	// Parameters
	double parameters_[MAXBROADENINGFUNCTIONPARAMS];
	// Whether function is inverted - y returns yFT, and vice versa
	bool inverted_;

	public:
	// Set function data
	void set(FunctionType function, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0, double p6 = 0.0);
	// Set function data from LineParser source
	bool set(LineParser& parser, int startArg);
	// Return function type
	FunctionType function() const;
	// Return number of parameters required
	int nParameters() const;
	// Return specified parameter
	double parameter(int index) const;
	// Return parameters array
	double* parameters();
	// Return specified parameter name
	const char* parameterName(int index) const;
	// Return short summary of function parameters
	CharString parameterSummary() const;
	// Set up any dependent parameters based on the current parameters
	void setUpDependentParameters();
	// Set inversion state
	void setInverted(bool state);
	// Return value of function given parameters x and omega
	double y(double x, double omega) const;
	// Return value of Fourier transform of function, given parameters x and omega
	double yFT(double x, double omega) const;
	// Return value of function given parameters x and omega, regardless of inversion state
	double yActual(double x, double omega) const;
	// Return value of Fourier transform of function, given parameters x and omega, regardless of inversion state
	double yFTActual(double x, double omega) const;


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Write data through specified LineParser
	bool write(LineParser& parser);
	// Read data through specified LineParser
	bool read(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif

