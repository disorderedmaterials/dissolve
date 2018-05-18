/*
	*** Gaussian Function Approximation
	*** src/math/gaussfit.cpp
	Copyright T. Youngs 2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/xydata.h"
#include "base/lineparser.h"
#include "math/gaussfit.h"
#include "templates/praxis.h"

// Constructor
GaussFit::GaussFit(XYData& sourceData) : sourceData_(sourceData)
{
	nGaussians_ = 0;
}

/*
 * Data
 */

// Generate full data from current parameters, ignoring all Gaussians whose width coefficient is below the provided value
void GaussFit::generateData(double minC)
{
	approximateData_.templateFrom(sourceData_);

	// Loop over defined Gaussians
	double A, c, c2, xCentre, x;
	for (int n=0; n<nGaussians_; ++n)
	{
		// Get and test limit of c
		c = c_.value(n);
		if (c < minC) continue;

		A = A_.value(n);
		xCentre = x_.value(n);
		c2 = c*c;

		for (int m=0; m<approximateData_.nPoints(); ++m)
		{
			x = approximateData_.x(m);

			// Approximation to original function
			approximateData_.addY(m, A * exp(-((x-xCentre)*(x-xCentre))/(2.0*c2)));
		}
	}
}

// Perform initial fit to source data, specifying number of Gaussians to use uniformly across source data range
double GaussFit::initialise(int nGaussians, double fwhm)
{
	// Set up Gaussian parameter arrays
	nGaussians_ = nGaussians;
	x_.initialise(nGaussians_);
	A_.initialise(nGaussians_);
	c_.initialise(nGaussians_);

	const double gaussDelta = (sourceData_.xLast() - sourceData_.xFirst()) / nGaussians - 1;

	// Set initial parameters
	for (int n=0; n<nGaussians_; ++n)
	{
		// Centre of Gaussian
		x_[n] = sourceData_.xFirst() + (n+0.5)*gaussDelta;

		// Amplitude (taken as approximate value of the source function)
		A_[n] = sourceData_.approximate((n+0.5)*gaussDelta);
	}

	// Width coefficient, based on supplied FWHM
	c_ = fwhm / (2.0 * sqrt(2.0 * log(2.0)));

	// Perform fit
	return reFit();
}

// Perform initial fit to source data, specifying spacing between Gaussians
double GaussFit::initialise(double gaussianSpacing, double fwhm)
{
	// Set up Gaussian parameter arrays
	nGaussians_ = (sourceData_.xLast() - sourceData_.xFirst()) / gaussianSpacing - 1;
	x_.initialise(nGaussians_);
	A_.initialise(nGaussians_);
	c_.initialise(nGaussians_);

	// Set initial parameters
	for (int n=0; n<nGaussians_; ++n)
	{
		// Centre of Gaussian
		x_[n] = sourceData_.xFirst() + (n+0.5)*gaussianSpacing;

		// Amplitude (taken as approximate value of the source function)
		A_[n] = sourceData_.approximate((n+0.5)*gaussianSpacing);
	}

	// Width coefficient, based on supplied FWHM
	c_ = fwhm / (2.0 * sqrt(2.0 * log(2.0)));

	// Perform fit
	return reFit();
}

// Construct suitable representation with minimal Gaussians automatically
double GaussFit::construct(double requiredError, int maxGaussians)
{
	// Clear any existing data
	x_.clear();
	A_.clear();
	c_.clear();
	nGaussians_ = 0;
	approximateData_.templateFrom(sourceData_);

	// Set up our additional data, including a smoothed version of the original function, and a 
	XYData smoothedData = sourceData_;
	smoothedData.kolmogorovZurbenkoFilter(3,3);

	double currentError = 100.0;

	// Make sure the approximate function is up to date
	generateData();

	// Step through the smoothed function seeking peaks / troughs in the data
	const int regionWidth = 3, regionDelta = regionWidth / 2;
	int lastSign = 0;
	double gradient, trialX, trialA, trialC;
	XYData trialData;
	trialData.templateFrom(sourceData_);
	Array<double>& trialDataY = trialData.arrayY();

	// Outer loop
	do
	{
		// Construct a smoothed representation of the sourceData_, with the current approximateData_ subtracted
		smoothedData.clear();
		for (int n=0; n<sourceData_.nPoints(); ++n) smoothedData.addPoint(sourceData_.x(n), sourceData_.y(n) - approximateData_.y(n));
		smoothedData.kolmogorovZurbenkoFilter(3,3);

		// Keep track of the number of Gaussians we add this cycle
		int nAdded = 0;

		// Go through smoothed data, calculating the gradient as we go, and seeking gradient minima (actually, crossovers between -ve and +ve gradients)
		for (int n=regionDelta; n<sourceData_.nPoints() - regionDelta; ++n)
		{
			// Calculate gradient at this point
			gradient = 0.0;
			for (int m=-regionDelta; m<regionDelta; ++m) gradient += (smoothedData.y(n+m+1) - smoothedData.y(n+m)) / (smoothedData.x(n+m+1) - smoothedData.x(n+m));

// 			Messenger::printVerbose("Gradient @ x = %f = %f\n", sourceData_.x(n), gradient);

			// Check sign of previous gradient vs the current one - do we add a Gaussian at this point?
			if ((lastSign != DUQMath::sgn(gradient)))
			{
				trialX = sourceData_.x(n);
				trialA = sourceData_.y(n) - approximateData_.y(n);
				trialC = 0.1;

				Messenger::printVerbose("Attempting Gaussian addition for peak/trough located at x = %f\n", trialX);

				// Set up minimiser, minimising test Gaussian only
				PrAxis<GaussFit> gaussMinimiser(*this, &GaussFit::costAmplitudeWidthXCentreStaticTrial);
				fitX_.clear();
				gaussMinimiser.addTarget(trialA);
				gaussMinimiser.addTarget(trialC);
				gaussMinimiser.addTarget(trialX);
// 				fitX_.add(trialX);
				gaussMinimiser.minimise(0.001, 0.1);

				// Generate trial data consisting of the current approximateData_ plus contribution from the trial Gaussian
				for (int m=0; m<sourceData_.nPoints(); ++m)
				{
					double x = sourceData_.x(m);
					trialDataY[m] = approximateData_.y(m) + trialA * exp(-((x-trialX)*(x-trialX))/(2*trialC*trialC));
				}

				// Have we decreased the overall error?
				double trialError = sourceData_.error(trialData);
				if (trialError < currentError)
				{
					Messenger::printVerbose("Accepting new Gaussian x = %f, A = %f, c = %f - error reduced from %f to %f\n", trialX, trialA, trialC, currentError, trialError);
					currentError = trialError;

					A_.add(trialA);
					x_.add(trialX);
					c_.add(trialC);
					++nGaussians_;
					++nAdded;

					// Store the trial data as the new approximate function
					approximateData_.arrayY() = trialDataY;

					// Check on error / nGaussians
					if (currentError <= requiredError) break;
					if (nGaussians_ == maxGaussians) break;
				}
				else Messenger::printVerbose("Rejecting new Gaussian x = %f, A = %f, c = %f - error increased from %f to %f\n", trialX, trialA, trialC, currentError, trialError);
			}

			// Store current sign of gradient
			lastSign = DUQMath::sgn(gradient);
		}

		// Check on error
		if (currentError <= requiredError)
		{
			Messenger::printVerbose("Required error threshold (%f) achieved - current error is %f, nGaussians = %i\n", requiredError, currentError, nGaussians_);
			break;
		}

		// Check on nGaussians
		if (nGaussians_ == maxGaussians)
		{
			Messenger::printVerbose("Maximum number of Gaussians (%i) reached - current error is %f\n", nGaussians_, currentError);
			break;
		}

		// If we added no Gaussians this cycle, bail out now
		if (nAdded == 0)
		{
			Messenger::printVerbose("No Gaussians added during last cycle, so exiting now.\n");
			break;
		}
	} while (1);

	return currentError;
}

// Set current parameters
bool GaussFit::set(const Array<double>& x, const Array<double>& A, const Array<double>& c)
{
	// Check sizes of provided Arrays
	int nInputGauss = x.nItems();
	if (A.nItems() != nInputGauss) return Messenger::error("Amplitude array does not match the size of the supplied xCentres array (%i vs %i items).\n", nInputGauss, A.nItems()); 
	if (c.nItems() != nInputGauss) return Messenger::error("Width coefficient array does not match the size of the supplied xCentres array (%i vs %i items).\n", nInputGauss, c.nItems());

	// Copy data
	nGaussians_ = nInputGauss;
	x_ = x;
	A_ = A;
	c_ = c;

	return true;
}

// Return number of Gaussians in fit
int GaussFit::nGaussians() const
{
	return nGaussians_;
}

// Return current function centres
const Array<double>& GaussFit::x() const
{
	return x_;
}

// Return current amplitudes
const Array<double>& GaussFit::A() const
{
	return A_;
}

// Return current C values
const Array<double>& GaussFit::c() const
{
	return c_;
}

// Save coefficients to specified file
bool GaussFit::saveCoefficients(const char* filename) const
{
	LineParser parser;
	if (!parser.openOutput(filename)) return false;

	parser.writeLineF("#  x  A  c\n");
	for (int n=0; n<nGaussians_; ++n) parser.writeLineF("%f  %f  %f\n", x_.value(n), A_.value(n), c_.value(n));

	parser.closeFiles();

	return true;
}

// Return approximate function
const XYData& GaussFit::approximation() const
{
	return approximateData_;
}

// Return Fourier transform of approximate function, ignoring all Gaussians whose width coefficient is below the provided value
XYData GaussFit::fourierTransform(double xMin, double xStep, double xMax, double cMin) const
{
	XYData ft;

	double A, c, c2, xCentre, x = xMin, y;
	while (x <= xMax)
	{
		y = 0.0;

		// Loop over defined Gaussians
		for (int n=0; n<nGaussians_; ++n)
		{
			// Get and test limit of c
			c = c_.value(n);
			if (c < cMin) continue;

			A = A_.value(n);
			xCentre = x_.value(n);
			c2 = c*c;

			y += A * exp(-(x*x*c*c)/2.0) * sin(xCentre*x)/(xCentre*x);
		}

		ft.addPoint(x, y);

		x += xStep;
	}

	return ft;
}

/*
 * Fitting
 */


// Re-fit to source data, starting from current parameters
double GaussFit::reFit()
{
	/*
	 * Our strategy for optimising the parameters with respect to the fit between approximate and source data will be as
	 * follows. Since the number of Gaussians may be reasonably large (N=hundreds, with 2N parameters total) attempting a
	 * multidimensional fit of this scope is going to be very slow. Since the Gaussians are only locally-dependent (in x)
	 * on each other, we can reduce the problem to a series of local minimisations of a smaller number of Gaussians at a
	 * time. To prevent discontinuities at the extremes of these ranges, we overlap the fit regions a little, and to get
	 * a good global fit we perform several loops of fitting over the data range (shifting the starting point a little
	 * each time).
	 */

	const int nLoops = 1;
	const int N = 10;
	const int overlap = 2;

	for (int loop=0; loop < nLoops; ++loop)
	{
		// Index of the Gaussian in the x_, A_, and c_ arrays is given by 'g'
		int g = loop*(N/nLoops);
		while (g < nGaussians_)
		{
			// Clear the reference array of X centres
			fitX_.clear();

			// Set up minimiser for the next batch
			PrAxis<GaussFit> gaussMinimiser(*this, &GaussFit::costAmplitudeWidth);

			// Add Gaussian parameters as fitting targets
			for (int n=0; n<N; ++n)
			{
				gaussMinimiser.addTarget(A_[g]);
				gaussMinimiser.addTarget(c_[g]);
				fitX_.add(x_[g]);

				// Increase Gaussian index - if that was the last one, break now
				++g;
				if (g == nGaussians_) break;
			}

			// Optimise this set of Gaussians
			gaussMinimiser.minimise(0.001, 0.1);

			// If we are not at the end of the Gaussian array, move the index backwards so the next set overlaps a little with this one
			if (g < nGaussians_) g -= overlap;
		}
	}

	// Calculate the approximate function
	generateData();

	return sourceData_.error(approximateData_);
}

/*
 * Cost Function Callbacks
 */

// Two-parameter cost function over full sourceData, using current approximateData_ and alpha array containing new trial A and c values
double GaussFit::costAmplitudeWidthStaticTrial(double* alpha, int nAlpha)
{
	double A = alpha[0], c2 = alpha[1]*alpha[1], xCentre = fitX_[0];

	double sose = 0.0, x, y, dy;

	// Dumb, but effective, check for c2 becoming zero
	if (c2 == 0.0) return 100.0;

	for (int n=0; n<sourceData_.nPoints(); ++n)
	{
		x = sourceData_.x(n);

		// Calculate current y - interpolated value of approximateData_ plus contribution from trial Gaussian
		y = approximateData_.y(n) + A * exp(-((x-xCentre)*(x-xCentre))/(2*c2));

		dy = sourceData_.y(n) - y;

		sose += dy*dy;
	}

	return sose;
}

// Three-parameter cost function over full sourceData, using current approximateData_ and alpha array containing new trial x, A and c values
double GaussFit::costAmplitudeWidthXCentreStaticTrial(double* alpha, int nAlpha)
{
	double A = alpha[0], c2 = alpha[1]*alpha[1], xCentre = alpha[2];

	double sose = 0.0, x, y, dy;

	// Dumb, but effective, check for c2 becoming zero
	if (c2 == 0.0) return 100.0;

	// Ensure xCentre never goes below zero
	if (xCentre < 0.0) return 100.0;

	for (int n=0; n<sourceData_.nPoints(); ++n)
	{
		x = sourceData_.x(n);

		// Calculate current y - interpolated value of approximateData_ plus contribution from trial Gaussian
		y = approximateData_.y(n) + A * exp(-((x-xCentre)*(x-xCentre))/(2*c2));

		dy = sourceData_.y(n) - y;

		sose += dy*dy;
	}

	return sose;
}

// Two-parameter cost function, with alpha array containing A and c values
double GaussFit::costAmplitudeWidth(double* alpha, int nAlpha)
{
	int nGauss = nAlpha / 2;

	// Construct working array of x values
	XYData sum;
	for (int m=0; m<sourceData_.nPoints(); ++m)
	{
		if (sourceData_.x(m) < fitX_[0]) continue;
		sum.addPoint(sourceData_.x(m), 0.0);
		if (sourceData_.x(m) > fitX_[nGauss-1]) break;
	}
	
	double A, c, c2, xCentre;
	for (int n=0; n<nGauss; ++n)
	{
		A = alpha[n*2];
		c = alpha[n*2+1];
		c2 = c*c;
		xCentre = fitX_[n];

		for (int m=0; m<sum.nPoints(); ++m) sum.addY(m, A * exp(-((sum.x(m)-xCentre)*(sum.x(m)-xCentre))/(2*c2)));
	}

	return sourceData_.error(sum);
}
