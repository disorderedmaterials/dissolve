/*
	*** Window Function
	*** src/math/windowfunction.h
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

#ifndef DISSOLVE_WINDOWFUNCTION_H
#define DISSOLVE_WINDOWFUNCTION_H

#define MAXWINDOWFUNCTIONPARAMS 6

#include "base/charstring.h"
#include "base/genericitembase.h"

// Forward Declarations
class LineParser;
class ProcessPool;
class XYData;

// Window Function
class WindowFunction : public GenericItemBase
{
	public:
	// Function Types
	enum FunctionType {
		NoWindow,		/* No window */
		BartlettWindow,		/* Bartlett (triangular) window */
		HannWindow,		/* von Hann (Hanning) window */
		LanczosWindow,		/* Lanczos window */
		NuttallWindow,		/* Nuttall window (continuous first derivatives over range) */
		SineWindow,		/* Sine Window */
		Lorch0Window,		/* Original Lorch function */
		nFunctionTypes		/* Number of defined WindowFunctions */
	};
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
	WindowFunction(FunctionType function = NoWindow, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0, double p6 = 0.0);
	// Destructor
	~WindowFunction();
	// Assignment Operator
	void operator=(const WindowFunction& source);


	/*
	 * Function Data
	 */
	private:
	// Function Type
	FunctionType function_;
	// Parameters
	double parameters_[MAXWINDOWFUNCTIONPARAMS];
	// Maximal x value for current data target
	double xMax_;

	public:
	// Set function data
	void set(FunctionType function, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0, double p6 = 0.0);
	// Set function data from LineParser source
	bool set(LineParser& parser, int startArg);
	// Return function type
	FunctionType function() const;
	// Return parameter specified
	double parameter(int n) const;
	// Return short summary of function parameters
	CharString parameterSummary() const;
	// Set-up function for specified data
	bool setUp(const XYData& data);
	// Return value of function given parameters x (current abscissa value) and omega (target abscissa value)
	double y(double x, double omega) const;


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

