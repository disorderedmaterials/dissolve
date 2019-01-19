/*
	*** Integrator
	*** src/math/integrator.cpp
	Copyright T. Youngs 2012-2019

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

#include "math/integrator.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "templates/array.h"

/*
 * Static Functions
 */

// Compute integral of supplied data via trapezoid rule
double Integrator::trapezoid(const Data1D& data)
{
	// Check for insufficient data
	if (data.nValues() < 2) return 0.0;

	// Grab data arrays
	const Array<double>& x = data.constXAxis();
	const Array<double>& y = data.constValues();

	double total = 0.0, y0 = y.firstValue(), y1, x0 = x.firstValue(), x1;
	for (int n=1; n<x.nItems(); ++n)
	{
		x1 = x.constAt(n);
		y1 = y.constAt(n);
		total += (x1 - x0) * (y0 + y1) * 0.5;
		x0 = x1;
		y0 = y1;
	}
	return total;
}

// Compute absolute integral of supplied data via trapezoid rule
double Integrator::absTrapezoid(const Data1D& data)
{
	// Check for insufficient data
	if (data.nValues() < 2) return 0.0;

	// Grab data arrays
	const Array<double>& x = data.constXAxis();
	const Array<double>& y = data.constValues();

	double total = 0.0, y0 = y.firstValue(), y1, x0 = x.firstValue(), x1;
	for (int n=1; n<x.nItems(); ++n)
	{
		x1 = x.constAt(n);
		y1 = y.constAt(n);
		total += fabs((x1 - x0) * (y0 + y1) * 0.5);
		x0 = x1;
		y0 = y1;
	}
	return total;
}

// Return sum of all values in supplied data
double Integrator::sum(const Data1D& data)
{
	// Grab data array
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n) total += values.constAt(n);

	return total;
}

// Return sum of all absolute values in supplied data
double Integrator::absSum(const Data1D& data)
{
	// Grab data array
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n) total += fabs(values.constAt(n));

	return total;
}

// Return sum of squares of all values in supplied data
double Integrator::sumOfSquares(const Data1D& data)
{
	// Grab data array
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n) total += values.constAt(n)*values.constAt(n);

	return total;
}

// Return sum of all values in supplied data
double Integrator::sum(const Data2D& data)
{
	// Grab data array
	const Array2D<double>& values = data.constValues2D();

	double total = 0.0;

	for (int n=0; n<values.linearArraySize(); ++n) total += values.constLinearValue(n);

	return total;
}

// Return sum of all absolute values in supplied data
double Integrator::absSum(const Data2D& data)
{
	// Grab data array
	const Array2D<double>& values = data.constValues2D();

	double total = 0.0;

	for (int n=0; n<values.linearArraySize(); ++n) total += fabs(values.constLinearValue(n));

	return total;
}

// Return sum of all values in supplied data
double Integrator::sum(const Data3D& data)
{
	// Grab data array
	const Array3D<double>& values = data.constValues3D();

	double total = 0.0;

	for (int n=0; n<values.linearArraySize(); ++n) total += values.constLinearValue(n);

	return total;
}

// Return sum of all absolute values in supplied data
double Integrator::absSum(const Data3D& data)
{
	// Grab data array
	const Array3D<double>& values = data.constValues3D();

	double total = 0.0;

	for (int n=0; n<values.linearArraySize(); ++n) total += fabs(values.constLinearValue(n));

	return total;
}
