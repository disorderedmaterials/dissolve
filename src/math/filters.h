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

// Forward Declarations
class XYData;

// Filters
class Filters
{
	public:
	// Apply median filter
	static void median(XYData& data, int length);
	// Perform moving average smoothing
	static void movingAverage(XYData& data, int avgSize);
	// Apply Kolmogorov–Zurbenko filter
	static void kolmogorovZurbenko(XYData& data, int k, int m);
	// Rebin onto uniform x axis
	static void reBin(XYData& data, double deltaX);
};

#endif
