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

// Compute integral of supplied data via trapezoid rule between the specified limits
double Integrator::trapezoid(const Data1D& data, double xMin, double xMax)
{
	// Check for insufficient data
	if (data.nValues() < 2) return 0.0;

	// Grab data arrays
	const Array<double>& x = data.constXAxis();
	const Array<double>& y = data.constValues();

	double total = 0.0, y0, y1, x0, x1;
	int nPoints = 0;
	for (int n=0; n<x.nItems(); ++n)
	{
		// Get current x and y values and check limit
		x1 = x.constAt(n);
		y1 = y.constAt(n);
		if (x1 < xMin) continue;
		if (x1 > xMax) break;

		// If this is the first point, don't try to increase integral
		if (nPoints > 0) total += (x1 - x0) * (y0 + y1) * 0.5;

		// Store old values
		x0 = x1;
		y0 = y1;

		++nPoints;
	}

	return total;
}

// Compute integral of supplied data via trapezoid rule within the specified range
double Integrator::trapezoid(const Data1D& data, const Range range)
{
	return trapezoid(data, range.minimum(), range.maximum());
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

// Return sum of supplied data between the specified limits
double Integrator::sum(const Data1D& data, double xMin, double xMax)
{
	// Grab data arrays
	const Array<double>& x = data.constXAxis();
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n)
	{
		if (x.constAt(n) < xMin) continue;
		if (x.constAt(n) > xMax) break;

		total += values.constAt(n);
	}

	return total;
}

// Return sum of supplied data within the specified range
double Integrator::sum(const Data1D& data, const Range range)
{
	return sum(data, range.minimum(), range.maximum());
}

// Return sum of all absolute values of supplied data
double Integrator::absSum(const Data1D& data)
{
	// Grab data array
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n) total += fabs(values.constAt(n));

	return total;
}

// Return sum of all absolute values between the specified limits
double Integrator::absSum(const Data1D& data, double xMin, double xMax)
{
	// Grab data arrays
	const Array<double>& x = data.constXAxis();
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n)
	{
		if (x.constAt(n) < xMin) continue;
		if (x.constAt(n) > xMax) break;

		total += fabs(values.constAt(n));
	}

	return total;
}

// Return sum of all absolute values within the specified range
double Integrator::absSum(const Data1D& data, const Range range)
{
	return absSum(data, range.minimum(), range.maximum());
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

// Return sum of squares of values between the specified limits
double Integrator::sumOfSquares(const Data1D& data, double xMin, double xMax)
{
	// Grab data arrays
	const Array<double>& x = data.constXAxis();
	const Array<double>& values = data.constValues();

	double total = 0.0;

	for (int n=0; n<values.nItems(); ++n)
	{
		if (x.constAt(n) < xMin) continue;
		if (x.constAt(n) > xMax) break;

		total += values.constAt(n)*values.constAt(n);
	}

	return total;
}

// Return sum of squares of values within the specified range
double Integrator::sumOfSquares(const Data1D& data, const Range range)
{
	return sumOfSquares(data, range.minimum(), range.maximum());
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
