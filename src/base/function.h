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

#include "base/charstring.h"

// Forward Declarations
class LineParser;
class ProcessPool;

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
	enum FunctionType { UnityFunction, GaussianFunction, DependentGaussianFunction, nFunctionTypes };
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
	void set(FunctionType function, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0, double p6 = 0.0);
	// Set function data from LineParser source
	bool set(LineParser& parser, int startArg);
	// Return function type
	FunctionType function() const;
	// Return short summary of function parameters
	CharString summary() const;
	// Return number of arguments accepted by function
	int nArguments() const;
	// Return value of function given parameter a
	double value(double a) const;
	// Return value of function given parameters a and b
	double value(double a, double b) const;
	// Return value of function given parameters a, b, and c
	double value(double a, double b, double c) const;
	// Return value of Fourier transform of function, given parameter a
	double ft(double a) const;
	// Return value of Fourier transform of function, given parameters a and b
	double ft(double a, double b) const;
	// Return value of Fourier transform of function, given parameters a, b, and c
	double ft(double a, double b, double c) const;
	// Return unity function
	static Function& unity();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
};

#endif

