/*
	*** Poisson Function Approximation
	*** src/math/poissonqfit.h
	Copyright T. Youngs 2018

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

#ifndef DISSOLVE_POISSONQFIT_H
#define DISSOLVE_POISSONQFIT_H

#include "templates/array.h"

// Forward Declarations
class XYData;

// Poisson Function Approximation to Q-Space Data (replicating EPSR's methodology)
class PoissonFit
{
	public:
	// Constructor / Destructor
	PoissonFit(const XYData& referenceQSpaceData, double rho);


	/*
	 * Data
	 */
	private:
	// Reference XYData, in Q-space, to which we are performing the fit
	const XYData& referenceData_;
	// Approximate (fitted) data
	XYData approximateData_;
	// Width coefficient for functions in real-space
	double sigmaR_;
	// Width coefficient for functions in reciprocal-space
	double sigmaQ_;
	// Minimum radius of generated potential (in real space)
	double rMin_;
	// Minimum radius of generated potential (in real space)
	double rMax_;
	// Stepsize between function centres (in real space)
	double rStep_;
	// Atomic density associated to reference data
	double rho_;
	// Number of functions used in fit
	int nPoissons_;
	// Function scale coefficients
	Array<double> C_;

	private:
	// Return function value at given x and specified parameters
	double functionValue(const double x, const int n) const;
	// Return Fourier transform of function at given Q index and specified parameters
	double functionFT(const int qIndex, const int n) const;
	// Generate full approximation from current parameters
	void generateApproximation();

	public:
	// Return approximate function
	const XYData& approximation() const;
	// Return Fourier transform of approximate function
	XYData fourierTransform(double gammaMin, double gammaStep, double gammaMax) const;
	// Set current parameters
	bool set(const Array<double>& C, double sigmaQ = 0.02, double sigmaR = 0.08);
	// Return number of functions in fit
	int nFunctions() const;
	// Return current C values
	const Array<double>& C() const;
	// Save coefficients to specified file
	bool saveCoefficients(const char* filename) const;


	/*
	 * Fitting
	 */
	private:
	// Difference function between reference and approximate datsets
	XYData referenceDelta_;
	// Current error
	double currentError_;
	// Precalculated terms
	Array<double> sqrtOnePlusQSqSigmaSq_, arcTanQSigma_, oneMinusQSqSigmaSq_;
	Array<int> n_, lnNPlusTwoFactorial_;
	double sigmaRCubed_;

	private:
	// Precalculate necessary terms
	void preCalculateTerms();

	public:
	// Construct suitable representation using givent number of Poissons spaced evenly over the real-space range specified
	double construct(int nPoissons, double rMin, double rMax, double sigmaQ, double sigmaR);
	// Re-fit to source data, starting from current parameters
	double reFit();


	/*
	 * Cost Functions
	 */
	private:
	// Cost function (all current C values)
	double costAllC(double* alpha, int nAlpha);
	double costAllC(const Array<double>& alpha);
};

#endif

