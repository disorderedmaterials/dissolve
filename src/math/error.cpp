/*
	*** Error Calculation
	*** src/math/error.cpp
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

#include "math/error.h"
#include "math/interpolator.h"
#include "math/xydata.h"
#include <algorithm>

using namespace std;

// Return RMSE between supplied data
double Error::rmse(const XYData& A, const XYData& B, bool quiet)
{
	// First, generate interpolation of data B
	Interpolator interpolatedB(B);

	// Grab x and y arrays from data A
	const Array<double>& aX = A.constX();
	const Array<double>& aY = A.constY();

	// Generate RMSE at x values of A
	double rmse = 0.0, delta;
	double firstX = 0.0, lastX = 0.0, x;
	int nPointsConsidered = 0;
	for (int n=0; n<aX.nItems(); ++n)
	{
		// Grab x value
		x = aX.constAt(n);

		// Is our x value lower than the lowest x value of the reference data?
		if (x < B.xMin()) continue;

		// Is our x value higher than the last x value of the reference data?
		if (x > B.xMax()) break;

		// Is this the first point considered?
		if (nPointsConsidered == 0) firstX = x;

		// Sum squared error
		delta = aY.constAt(n) - interpolatedB.y(x);
		rmse += delta*delta;
		lastX = x;
		++nPointsConsidered;
	}

	// Finalise RMSE and summarise result
	rmse = sqrt(rmse/nPointsConsidered);
	if (!quiet) Messenger::print("RMSE between datasets is %15.9e over %15.9e < x < %15.9e (%i points).\n", rmse, firstX, lastX, nPointsConsidered);

	return rmse;
}

// Return percentage error between supplied data
double Error::percent(const XYData& A, const XYData& B, bool quiet)
{
	// First, generate interpolation of data B
	Interpolator interpolatedB(B);

	// Grab x and y arrays from data A
	const Array<double>& aX = A.constX();
	const Array<double>& aY = A.constY();

	/*
	 * Generate error estimate over actual values of our own data
	 * We will calculate the mean absolute percentage error (MAPE) as well as the simple sum of errors divided by sum of reference data.
	 * We do both, and return the higher of the two, since the MAPE is not a good measure when considering very spiky data (since the
	 * on-step forecast is often worse than the actual error of the predicted value.
	 */
	// TODO CHANGE THIS TO BE PURE MAPE?
	
	double sume = 0.0, sumf = 0.0, sumy = 0.0;
	double firstX = 0.0, lastX = 0.0, x, y;
	int nPointsConsidered = 0;
	for (int n=0; n<aX.nItems(); ++n)
	{
		// Grab x value
		x = aX.constAt(n);

		// Is our x value lower than the lowest x value of the reference data?
		if (x < B.xMin()) continue;

		// Is our x value higher than the last x value of the reference data?
		if (x > B.xMax()) break;

		// Is this the first point considered?
		if (nPointsConsidered == 0) firstX = x;

		y = aY.constAt(n);

		// Accumulate numerator - sum of forecast errors
		sume += fabs(y - interpolatedB.y(x));
		sumy += fabs(interpolatedB.y(x));

		// Accumulate denominator - one-step naive forecast (backwards forecast for first point)
		if (nPointsConsidered > 0) sumf += fabs(y - aY.constAt(n-1));
		else if (n < aX.nItems()-1) sumf += fabs(y - aY.constAt(n+1));

		lastX = x;
		++nPointsConsidered;
	}

	// Finalise MAPE and summarise result
	double denominator;
	// Normalisation to N/(N-1) not performed when using one-step backwards forecast for first point.
// 	if (sumf > 0.0) denominator = (double(nPointsConsidered) / double(nPointsConsidered-1)) * sumf;
	if (sumf > 0.0) denominator = sumf;
	else denominator = 1.0;
	double mape = sume / denominator;
	Messenger::printVerbose("MAPE between datasets is %7.3f%% over %15.9e < x < %15.9e (%i points), d(|Y|)/sum(|Y|) = %7.3f%%.\n", mape, firstX, lastX, nPointsConsidered, sume/sumy);

	return max(mape,sume/sumy);
}

