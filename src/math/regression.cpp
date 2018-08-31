/*
	*** Regression
	*** src/math/regression.cpp
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

#include "math/regression.h"
#include "math/xydata.h"
#include "templates/array.h"

// Return gradient of last n points
double Regression::linear(const XYData& data, int nSamples)
{
	double yMean;
	return linear(data, nSamples, yMean);
}

// Return gradient of last n points, along with average y value
double Regression::linear(const XYData& data, int nSamples, double& yBar)
{
	// Grab data arrays
	const Array<double>& x = data.constX();
	const Array<double>& y = data.constY();

	double Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
	double xBar = 0.0;
	yBar = 0.0;

	// Calculate mean values of x and y
	for (int n=data.nDataPoints()-nSamples; n<data.nDataPoints(); ++n)
	{
		xBar += x.constAt(n);
		yBar += y.constAt(n);
	}
	xBar /= nSamples;
	yBar /= nSamples;

	// Determine Sx, Sy, and Sxy
	double dx, dy;
	for (int n=data.nDataPoints()-nSamples; n<data.nDataPoints(); ++n)
	{
		dx = x.constAt(n) - xBar;
		dy = y.constAt(n) - yBar;
		Sxx += dx * dx;
		Syy += dy * dy;
		Sxy += dx * dy;
	}

	// Return the gradient
	return Sxy / Sxx;
}
