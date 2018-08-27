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

// Apply median filter to data
void Filters::median(XYData& data, int length)
{
	double data[length], avg, result;
	int m, i = length/2, n = length/2, miny, maxy;
	Array<double> newY(x_.nItems());
	
	// Set boundary values
	for (m=0; m<n; ++m)
	{
		newY[m] = y_[m];
		newY[x_.nItems()-1-m] = y_[m];
	}

	// Now loop over remaining points
	while (n < (x_.nItems()-i))
	{
		// Grab data values, and determine min/max values
		miny = 0;
		maxy = 0;
		for (m=-i; m<=i; ++m)
		{
			data[m+i] = y_[n+m];
			if (data[m+i] < data[miny]) miny = m+i;
			if (data[m+i] > data[maxy]) maxy = m+i;
		}

		// Determine median value (without sorting)
		// First, calculate average value
		avg = 0.0;
		for (m=0; m<length; ++m) if ((m != miny) && (m != maxy)) avg += data[m];
		avg /= length-2;
		// Now find value closest to the average
		result = data[0];
		for (m=0; m<length; ++m) if ((m != miny) && (m != maxy) && (fabs(data[m]-avg) < fabs(result-avg))) result = data[m];
		
		// Store median value
		newY[n] = result;
		
		++n;
	}
	
	// Store new values
	y_ = newY;

	interpolationInterval_ = -1;
}

// Moving average smoothing
void Filters::movingAverage(XYData& data, int avgSize)
{
	// Make sure avgSize is odd
	if (avgSize%2 == 0) --avgSize;

	Array<double> newY(x_.nItems());
	newY = 0.0;
	int n, m, i = avgSize/2;

	// Left-most region of data
	for (n=0; n<i; ++n)
	{
		for (m=0; m<=n+i; ++m) newY[n] += y_[m];
		newY[n] /= (i + 1 + n);
	}

	// Central region (full average width available)
	for (n=i; n < x_.nItems()-i; ++n)
	{
		for (m=n-i; m <= n+i; ++m) newY[n] += y_[m];
		newY[n] /= avgSize;
	}

	// Right-most region of data
	for (n=x_.nItems()-i; n<x_.nItems(); ++n)
	{
		for (m=n-i; m<x_.nItems(); ++m) newY[n] += y_[m];
		newY[n] /= (x_.nItems() - n + i + 1);
	}

	y_ = newY;

	interpolationInterval_ = -1;
}

// Kolmogorov–Zurbenko filter
void Filters::kolmogorovZurbenko(XYData& data, int k, int m)
{
	for (int iteration=0; iteration<k; ++iteration) movingAverage(data, m);
}


// Rebin onto uniform x axis
void Filters::reBin(XYData& data, double deltaX)
{
	// If deltaX is negative, work out a deltaX to use
	if (deltaX < 0.0)
	{
		deltaX = 0.0;
		for (int n=1; n<x_.nItems(); ++n) deltaX += x_[n]-x_[n-1];
		deltaX /= x_.nItems()-1;
		deltaX *= 0.5;
	}

	// Interpolate the existing data
	interpolate(XYData::LinearInterpolation);

	// Generate new data
	XYData rebinnedData;
	double x = 0.0, xLimit = xMax();
	while (x < xLimit)
	{
		rebinnedData.addPoint(x, interpolated(x));
		x += deltaX;
	}

	// Overwrite old data
	(*this) = rebinnedData;

	interpolationInterval_ = -1;
}
