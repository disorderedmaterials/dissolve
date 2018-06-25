/*
	*** Poisson Function Approximation
	*** src/math/poissonqfit.cpp
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

#include "base/xydata.h"
#include "base/lineparser.h"
#include "math/poissonfit.h"
#include "math/mc.h"
#include "math/praxis.h"

// Constructor
PoissonFit::PoissonFit(const XYData& referenceData, double rho) : referenceData_(referenceData)
{
	rho_ = rho;

	approximateData_.templateFrom(referenceData_);

	nPoissons_ = 0;
}

/*
 * Data
 */

// Return function value at given x and specified parameters
double PoissonFit::functionValue(const double x, const int n) const
{
	double factor = 1.0 / (4.0 * PI * rho_ * sigmaRCubed_ * lnNPlusTwoFactorial_.value(n));

	double value = pow(x / sigmaR_, n) * exp(-x/sigmaR_);

	return factor * value;
}

// Return Fourier transform at given Q index and specified parameters
double PoissonFit::functionFT(const int qIndex, const int n) const
{
	double na = n * arcTanQSigma_.value(qIndex);

	double factor = 1.0 / ( (n+2) * pow(sqrtOnePlusQSqSigmaSq_.value(qIndex),n+4) );

	double value = 2.0 * cos(na) + (oneMinusQSqSigmaSq_.value(qIndex) / (referenceData_.x(qIndex)*sigmaQ_))*sin(na);

	return factor * value;
}

// Generate full approximation from current parameters
void PoissonFit::generateApproximation()
{
	approximateData_.arrayY() = 0.0;

	double y;

	// Loop over points in Q
	for (int m=0; m<approximateData_.nPoints(); ++m)
	{
		y = 0.0;

		// Sum values of functions at this Q value
		for (int n=1; n<nPoissons_; ++n)
		{
			y += C_[n] * functionFT(m, n);
		}

		approximateData_.addY(m, y);
	}
}

// Set current parameters
bool PoissonFit::set(const Array<double>& C, double sigmaQ, double sigmaR)
{
	// Copy data
	C_ = C;
	nPoissons_ = C_.nItems();
	sigmaQ_ = sigmaQ;
	sigmaR_ = sigmaR;

	printf("NP = %i, sq = %f, sr = %f\n", nPoissons_, sigmaQ_, sigmaR_);
	// Recalculate coefficients
	preCalculateTerms();

	// Regenerate approximation
	generateApproximation();

	return true;
}

// Return number of functions in fit
int PoissonFit::nFunctions() const
{
	return nPoissons_;
}

// Return current C values
const Array<double>& PoissonFit::C() const
{
	return C_;
}

// Save coefficients to specified file
bool PoissonFit::saveCoefficients(const char* filename) const
{
	LineParser parser;
	if (!parser.openOutput(filename)) return false;

	parser.writeLineF("#   C\n");
	for (int n=0; n<nPoissons_; ++n) parser.writeLineF("%f\n", C_.value(n));

	parser.closeFiles();

	return true;
}

// Return approximate function
const XYData& PoissonFit::approximation() const
{
	return approximateData_;
}

// Return Fourier transform of approximate function
XYData PoissonFit::fourierTransform(double gammaMin, double gammaStep, double gammaMax) const
{
	XYData ft;

	double x = gammaMin, y;
	while (x <= gammaMax)
	{
		y = 0.0;

		// Loop over defined Poissons
		for (int n=1; n<nPoissons_; ++n) y += C_.value(n) * functionValue(x, n);

		ft.addPoint(x, y);

		x += gammaStep;
	}

	return ft;
}

/*
 * Fitting
 */

// Precalculate necessary terms
void PoissonFit::preCalculateTerms()
{
	// TODO Could extend these to 2D arrays to include 'n' dimension

	// Factors involving Q
	const Array<double>& Q = referenceData_.constArrayX();
	sqrtOnePlusQSqSigmaSq_.initialise(referenceData_.nPoints());
	oneMinusQSqSigmaSq_.initialise(referenceData_.nPoints());
	arcTanQSigma_.initialise(referenceData_.nPoints());
	for (int n=0; n<referenceData_.nPoints(); ++n)
	{
		sqrtOnePlusQSqSigmaSq_[n] = sqrt(1.0 + Q.value(n)*Q.value(n) * sigmaQ_*sigmaQ_);
		oneMinusQSqSigmaSq_[n] = 1.0 - Q.value(n)*Q.value(n) * sigmaQ_*sigmaQ_;
		arcTanQSigma_[n] = atan(Q.value(n)*sigmaQ_);
	}

	// Poisson function powers (n)
	n_.initialise(nPoissons_);
	for (int i=0; i<nPoissons_; ++i)
	{
		double d = i*rStep_;
		if ((sigmaR_ > 0.0) && (sigmaR_ <= d)) n_[i] = d / sigmaR_ - 1;
		else n_[i] = 0;
	}
	lnNPlusTwoFactorial_.initialise(nPoissons_);
	for (int i=1; i<nPoissons_; ++i) lnNPlusTwoFactorial_[i] = lgamma(n_.value(i));
	sigmaRCubed_ = sigmaR_*sigmaR_*sigmaR_;
}

// Construct suitable representation using givent number of Poissons spaced evenly over the real-space range specified
double PoissonFit::construct(int nPoissons, double rMin, double rMax, double sigmaQ, double sigmaR)
{
	// Clear any existing data
	nPoissons_ = nPoissons;
	C_.initialise(nPoissons_);
	sigmaQ_ = sigmaQ;
	sigmaR_ = sigmaR;
	rMin_ = rMin;
	rMax_ = rMax;
	rStep_ = (rMax_ - rMin_) / nPoissons_;

	// Pre-calculate the necessary terms
	preCalculateTerms();

	// Clear the approximate data
	approximateData_.templateFrom(referenceData_);

	// Set up minimiser
// 	MonteCarloMinimiser<PoissonFit> minimiser(*this, &PoissonFit::costAllC);
	C_ = 0.1;
// 	currentError_ = minimiser.minimise(C_, 1000);

// 	PrAxis<PoissonFit> minimiser(*this, &PoissonFit::costAllC);
// 	C_ = 0.1;
// 	for (int n=0; n<nPoissons_; ++n) minimiser.addTarget(C_[n]);
// 	currentError_ = minimiser.minimise(0.01);

	// Make sure the approximate function is up-to-date
	generateApproximation();

	return currentError_;
}

// Re-fit to source data, starting from current parameters
double PoissonFit::reFit()
{
	// TODO
}

/*
 * Cost Function Callbacks
 */

// Cost function (all current C values)
double PoissonFit::costAllC(double* alpha, int nAlpha)
{
	approximateData_.arrayY() = 0.0;

	double y;

	// Loop over points in Q
	for (int m=0; m<approximateData_.nPoints(); ++m)
	{
		y = 0.0;

		// Sum values of functions at this Q value
		for (int n=0; n<nAlpha; ++n)
		{
			y += alpha[n] * functionFT(m, n);
		}

		approximateData_.addY(m, y);
	}

	return referenceData_.error(approximateData_);
}

double PoissonFit::costAllC(const Array<double>& alpha)
{
	double y, sose = 0.0;

	// Loop over points in Q
	for (int m=0; m<referenceData_.nPoints(); ++m)
	{
		y = 0.0;

		// Sum values of functions at this Q value
		for (int n=0; n<alpha.nItems(); ++n)
		{
			y += alpha.value(n) * functionFT(m, n);
		}

		sose += (referenceData_.y(m) - y)*(referenceData_.y(m) - y);
	}

	return sose;
}
