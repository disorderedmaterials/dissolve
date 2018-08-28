/*
	*** Filters
	*** src/math/filters.h
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

#ifndef DISSOLVE_FILTERS_H
#define DISSOLVE_FILTERS_H

#include "math/broadeningfunction.h"

// Forward Declarations
class XYData;

// Filters
class Filters
{
	public:
	// Apply median filter to data
	static void medianFilter(XYData& data, int length);
	// Perform moving average smoothing on data
	static void movingAverage(XYData& data, int avgSize);
	// Apply Kolmogorov–Zurbenko filter to data
	static void kolmogorovZurbenkoFilter(XYData& data, int k, int m);
	// Perform point-wise convolution of data with the supplied BroadeningFunction
	static void convolve(XYData& data, BroadeningFunction function);
	// Perform point-wise convolution of data with the supplied BroadeningFunction, normalising to the original integral of the function
	static void convolveNormalised(XYData& data, BroadeningFunction function);
	// Subtract average level (starting at supplied x value) from data
	static double subtractAverage(XYData& data, double xStart);
};

#endif
