/*
	*** FitKernel - Steepest Descent Minimiser
	*** src/gui/uchroma/kernels/fit_sd.cpp
	Copyright T. Youngs 2012-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/kernels/fit.h"

// Steepest Descent Minimise
bool FitKernel::sdMinimise(Array<double>& alpha)
{
	// Control variables
	double gradientDelta = 0.01;

	// Create initial gradient
	Array<double> gradient(alpha.nItems()), tempAlpha;
	for (int n=0; n<alpha.nItems(); ++n)
	{
		tempAlpha = alpha;
		tempAlpha[n] = (1.0+gradientDelta) * alpha[n];
		gradient[n] = rmsError(tempAlpha);
		tempAlpha[n] = (1.0-gradientDelta) * alpha[n];
		gradient[n] -= rmsError(tempAlpha);
	}
	gradient /= (2.0*gradientDelta);

	// Set initial stepsize
	double lambda = 1.0;

	// Get initial cost
	double oldRMSE = rmsError(alpha);
	Messenger::print("Initial RMSE = %e\n", oldRMSE);

	// Do some iterations
	int n;
	double currentRMSE, deltaRMSE;
	for (int step=1; step<=maxSteps_; ++step)
	{
		// Test stepsize - if it does not decrease the RMSE, decrease it until it does
		do
		{
			// Update alpha and get RMSE
			for (n=0; n<alpha.nItems(); ++n) tempAlpha[n] = alpha[n] - gradient[n]*lambda;
			currentRMSE = rmsError(tempAlpha);

			// Current RMSE higher than old RMSE?
			deltaRMSE = currentRMSE - oldRMSE;
			if (deltaRMSE > 0.0) lambda *= 0.5;
		} while (deltaRMSE > 0.0);
		alpha = tempAlpha;

		// Check on convergence tolerance
		if (fabs(deltaRMSE) < tolerance_)
		{
			Messenger::print("SD converged (tolerance = %e, delta(RMSE) = %e)\n", tolerance_, deltaRMSE);
			break;
		}

		// Generate new gradient
		for (n=0; n<alpha.nItems(); ++n)
		{
			tempAlpha = alpha;
			tempAlpha[n] = (1.0+gradientDelta) * alpha[n];
			gradient[n] = rmsError(tempAlpha);
			tempAlpha[n] = (1.0-gradientDelta) * alpha[n];
			gradient[n] -= rmsError(tempAlpha);
		}
		gradient /= (2.0*gradientDelta);

		oldRMSE = currentRMSE;
	}

	// Get final cost
	Messenger::print("Final RMSE = %e\n", rmsError(alpha));
	
	return true;
}
