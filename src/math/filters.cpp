/*
	*** Filters
	*** src/math/filters.cpp
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

#include "math/filters.h"
#include "math/integrator.h"
#include "math/data1d.h"
#include "templates/array.h"

// Apply median filter to data
void Filters::medianFilter(Data1D& data, int length)
{
	// Grab y array
	Array<double>& y = data.values();

	double window[length], avg, result;
	int m, i = length/2, n = length/2, miny, maxy;
	Array<double> newY(data.nValues());
	
	// Set boundary values
	for (m=0; m<n; ++m)
	{
		newY[m] = y[m];
		newY[data.nValues()-1-m] = y[data.nValues()-1-m];
	}

	// Now loop over remaining points
	while (n < (data.nValues()-i))
	{
		// Grab data values, and determine min/max values
		miny = 0;
		maxy = 0;
		for (m=-i; m<=i; ++m)
		{
			window[m+i] = y[n+m];
			if (window[m+i] < window[miny]) miny = m+i;
			if (window[m+i] > window[maxy]) maxy = m+i;
		}

		// Determine median value without sorting. First, calculate average value...
		avg = 0.0;
		for (m=0; m<length; ++m) if ((m != miny) && (m != maxy)) avg += window[m];
		avg /= length-2;

		// Now find value closest to the average
		result = window[0];
		for (m=0; m<length; ++m) if ((m != miny) && (m != maxy) && (fabs(window[m]-avg) < fabs(result-avg))) result = window[m];
		
		// Store median value
		newY[n] = result;
		
		++n;
	}
	
	// Store new values
	y = newY;
}

// Perform moving average smoothing
void Filters::movingAverage(Data1D& data, int avgSize)
{
	// Grab y array
	Array<double>& y = data.values();

	// Make sure avgSize is odd
	if (avgSize%2 == 0) --avgSize;

	Array<double> newY(data.nValues());
	newY = 0.0;
	int n, m, i = avgSize/2;

	// Left-most region of data
	for (n=0; n<i; ++n)
	{
		for (m=0; m<=n+i; ++m) newY[n] += y[m];
		newY[n] /= (i + 1 + n);
	}

	// Central region (full average width available)
	for (n=i; n < data.nValues()-i; ++n)
	{
		for (m=n-i; m <= n+i; ++m) newY[n] += y[m];
		newY[n] /= avgSize;
	}

	// Right-most region of data
	for (n=data.nValues()-i; n<data.nValues(); ++n)
	{
		for (m=n-i; m<data.nValues(); ++m) newY[n] += y[m];
		newY[n] /= (data.nValues() - n + i + 1);
	}

	y = newY;
}

// Apply Kolmogorov–Zurbenko filter
void Filters::kolmogorovZurbenkoFilter(Data1D& data, int k, int m)
{
	for (int iteration=0; iteration<k; ++iteration) movingAverage(data, m);
}

// Perform point-wise convolution of this data with the supplied BroadeningFunction
void Filters::convolve(Data1D& data, BroadeningFunction function)
{
	// Grab x and y arrays
	const Array<double>& x = data.constXAxis();
	Array<double>& y = data.values();

	Array<double> newY(data.nValues());

	// Outer loop over existing data points
	double xCentre, xBroad;
	for (int n=0; n<x.nItems(); ++n)
	{
		// Grab x value as our current xCentre
		xCentre = x.constAt(n);

		// Inner loop over whole array
		for (int m=0; m<x.nItems(); ++m)
		{
			xBroad = x.constAt(m) - xCentre;
			newY[m] += y.constAt(n) * function.y(xBroad, 0.0);
		}
	}

	y = newY;
}

// Perform point-wise convolution of this data with the supplied BroadeningFunction, normalising to the original integral of the function
void Filters::convolveNormalised(Data1D& data, BroadeningFunction function)
{
	// Calculate the original integral
	double originalIntegral = Integrator::absTrapezoid(data);

	// If the original integral is zero, nothing to do
	if (originalIntegral == 0.0) return;

	// Convolve the function
	convolve(data, function);

	// Calculate the new integral
	double newIntegral = Integrator::absTrapezoid(data);

	data.values() *= (originalIntegral / newIntegral);
}

// Subtract average level from data, forming average from supplied x value
double Filters::subtractAverage(Data1D& data, double xStart)
{
	// Grab x and y arrays
	const Array<double>& x = data.constXAxis();
	Array<double>& y = data.values();

	double sum = 0.0;
	int nPoints = 0;
	for (int n=0; n<x.nItems(); ++n)
	{
		if (x.constAt(n) >= xStart)
		{
			sum += y[n];
			++nPoints;
		}
	}

	y -= sum / nPoints;

	return sum / nPoints;
}
