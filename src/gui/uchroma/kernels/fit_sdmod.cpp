/*
	*** FitKernel - Modified Steepest Descent Minimiser
	*** src/gui/uchroma/kernels/fit_sdmod.cpp
	Copyright T. Youngs 2012-2019

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
#include "math/mathfunc.h"

// Modified Steepest Descent Minimiser
bool FitKernel::sdModMinimise(Array<double>& alpha, double randomMin, double randomMax)
{
	// Control variables
	double gradientDelta = 0.01;
	const int maxIterations = 100;
	const double factor = 0.50;
	Array<double> gradient(alpha.nItems()), tempAlpha(alpha.nItems());
	double currentRMSE, deltaRMSE, lambda = 1.0;
	bool converged = false;
	int n, i;

	// Loop over whole routine (up to maxSteps)
	int step = 0;
	do
	{
		// Get initial cost from supplied alpha
		double oldRMSE = rmsError(alpha);
		Messenger::print("Initial RMSE = %e\n", oldRMSE);

		// Check for RMSE being less than current tolerance
		if (oldRMSE < tolerance_)
		{
			Messenger::print("Current RMSE (%e) is below tolerance value (%e) so no point continuing minimisation.\n", oldRMSE, tolerance_);
			break;
		}

		// Randomise initial parameters to try and find better initial set?
		if ((modSDNRandomTrials_ > 0) && (step < modSDNRandomTrials_))
		{
			Messenger::print("Generating %i random values per alpha, within the range %e < alpha < %e.\n", modSDNRandomTrials_, randomMin, randomMax);
			for (n=0; n<alpha.nItems(); ++n)
			{
				tempAlpha = alpha;
				for (i=0; i<modSDNRandomTrials_; ++i)
				{
					tempAlpha[n] = DissolveMath::random() * (randomMax - randomMin) + randomMin;
					currentRMSE = rmsError(tempAlpha);
					if (currentRMSE < oldRMSE)
					{
						Messenger::print("Replaced current alpha with random parameters (deltaRMSE = %f).\n", currentRMSE - oldRMSE);
						oldRMSE = currentRMSE;
						alpha = tempAlpha;
					}
				}
			}
		}

		// Create initial gradient
		for (n=0; n<alpha.nItems(); ++n)
		{
			tempAlpha = alpha;
			tempAlpha[n] = (1.0+gradientDelta) * alpha[n];
			gradient[n] = rmsError(tempAlpha);
			tempAlpha[n] = (1.0-gradientDelta) * alpha[n];
			gradient[n] -= rmsError(tempAlpha);
		}
		gradient /= (2.0*gradientDelta);

		// Go!
		// Do some iterations
		do
		{
			++step;

			// Setup scaling factors for alpha
			double maxGrad = fabs(gradient[n]);
			for (n=0; n<alpha.nItems(); ++n) if (fabs(gradient[n]) > maxGrad) maxGrad = fabs(gradient[n]);
			gradient /= maxGrad;
			for (n=0; n<alpha.nItems(); ++n) if (fabs(gradient[n]) < 0.01) gradient[n] = 0.0;

			bool reduced = false;
			double lastDeltaRMSE = 1.0;
			int nSwaps = 0;
			bool foundPoint = false;
			for (i=0; i<maxIterations; ++i)
			{
				if ((fabs(lambda) < 1.0e-4) && (deltaRMSE > 0.0))
				{
					lambda = (lambda < 0.0 ? 1.0 : -1.0);
					if (++nSwaps > 2) break;
				}
				// Update alpha and get RMSE
				for (n=0; n<alpha.nItems(); ++n) tempAlpha[n] = alpha[n] - gradient[n]*lambda;
				currentRMSE = rmsError(tempAlpha);

				// Current RMSE higher than old RMSE?
				deltaRMSE = currentRMSE - oldRMSE;
				if (deltaRMSE > 0.0)
				{
					lambda *= factor;
					reduced = true;
	// 				printf("   -- %03i Reduce (deltaRMSE = %f), lambda = %f \n", i, deltaRMSE, lambda);
				}
				else if (!reduced)
				{
					// If we have made things worse, revert to the previous cost
					if (deltaRMSE > lastDeltaRMSE)
					{
						lambda *= factor;
	// 					printf("   -- %03i Worse delta (%f), so reverting to lambda = %f \n", i, deltaRMSE, lambda);
						foundPoint = true;
						break;
					}
					// First iteration, and we have reduced the RMSE, so double lambda and try again...
					lambda /= factor;
					lastDeltaRMSE = deltaRMSE;
	// 				printf("   -- %03i Incrse (deltaRMSE = %f), lambda = %f \n", i, deltaRMSE, lambda);
				}
				else
				{
					foundPoint = true;
					break;
				}
			}

			// Did we actually manage to reduce the RMSE?
			if (!foundPoint)
			{
				Messenger::print("Step %04i RMSE = %e (no better step found)\n", step, oldRMSE);
				break;
			}

			// Calculate new RMSE from current lambda
			for (n=0; n<alpha.nItems(); ++n) alpha[n] = alpha[n] - gradient[n]*lambda;
			currentRMSE = rmsError(alpha);
			deltaRMSE = currentRMSE - oldRMSE;

			// Check on convergence tolerance
			if (fabs(deltaRMSE) < tolerance_)
			{
				Messenger::print("Step %04i RMSE = %e (delta = %e) [CONVERGED, tolerance = %e]\n", step, currentRMSE, deltaRMSE, tolerance_);
				converged = true;
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
			};
			gradient /= (2.0*gradientDelta);

			oldRMSE = currentRMSE;
			Messenger::print("Step %04i RMSE = %e (delta = %e)\n", step, oldRMSE, deltaRMSE);
		} while (step < maxSteps_);

		// Check for convergence
		if (converged) break;

	} while (step < maxSteps_);

	// Get final cost
	Messenger::print("Final RMSE = %e\n", rmsError(alpha));
	
	return true;
}
