/*
	*** Gaussian Function Approximation
	*** src/math/gaussfit.cpp
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
#include "math/gaussfit.h"
#include "templates/praxis.h"

// Constructor
GaussFit::GaussFit(const XYData& referenceData, int kzSmoothK, int kzSmoothM)
{
	nGaussians_ = 0;

	// Copy reference data, and smooth it if requested
	referenceData_ = referenceData;
	if (kzSmoothK > 0) referenceData_.kolmogorovZurbenkoFilter(kzSmoothK, kzSmoothM);
}

/*
 * Data
 */

// Generate full data from current parameters, ignoring all Gaussians whose width coefficient is below the provided value
void GaussFit::generateData(double minC)
{
	approximateData_.templateFrom(referenceData_);

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

	const double gaussDelta = (referenceData_.xLast() - referenceData_.xFirst()) / nGaussians - 1;

	// Set initial parameters
	for (int n=0; n<nGaussians_; ++n)
	{
		// Centre of Gaussian
		x_[n] = referenceData_.xFirst() + (n+0.5)*gaussDelta;

		// Amplitude (taken as approximate value of the source function)
		A_[n] = referenceData_.approximate((n+0.5)*gaussDelta);
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
	nGaussians_ = (referenceData_.xLast() - referenceData_.xFirst()) / gaussianSpacing - 1;
	x_.initialise(nGaussians_);
	A_.initialise(nGaussians_);
	c_.initialise(nGaussians_);

	// Set initial parameters
	for (int n=0; n<nGaussians_; ++n)
	{
		// Centre of Gaussian
		x_[n] = referenceData_.xFirst() + (n+0.5)*gaussianSpacing;

		// Amplitude (taken as approximate value of the source function)
		A_[n] = referenceData_.approximate((n+0.5)*gaussianSpacing);
	}

	// Width coefficient, based on supplied FWHM
	c_ = fwhm / (2.0 * sqrt(2.0 * log(2.0)));

	// Perform fit
	return reFit();
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

// Save Fourier-transformed Gaussians to individual files
bool GaussFit::saveFTGaussians(const char* filenamePrefix, double xStep) const
{
	double xDelta = (xStep < 0.0 ? referenceData_.x(1) - referenceData_.xFirst() : xStep);
	for (int n=0; n<nGaussians_; ++n)
	{
		LineParser parser;
		if (!parser.openOutput(CharString("%s-%03i.gauss", filenamePrefix, n))) return false;

		double xCentre = x_.value(n);
		double A = A_.value(n);
		double c = c_.value(n);
		if (!parser.writeLineF("#  x=%f  A=%f  c=%f\n", xCentre, A, c)) return false;

		double x = referenceData_.xFirst();
		while (x < referenceData_.xLast())
		{
			parser.writeLineF("%f  %f\n", x, A * exp(-(x*x*c*c)/2.0) * sin(xCentre*x)/(xCentre*x));
			x += xDelta;
		}

		parser.closeFiles();
	}

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

// Construct suitable representation with minimal Gaussians automatically
double GaussFit::construct(double requiredError, int maxGaussians)
{
	// Clear any existing data
	x_.clear();
	A_.clear();
	c_.clear();
	nGaussians_ = 0;
	approximateData_.templateFrom(referenceData_);

	// Make sure the approximate function is up to date
	generateData();

	// Step through the smoothed function seeking peaks / troughs in the data
	const int regionWidth = 5, regionDelta = regionWidth / 2;
	int lastSign = 0;
	double gradient, trialX, trialA, trialC;

	// Calculate starting sum-of-squares error
	currentError_ = referenceData_.sumOfSquares();

	// Outer loop
	do
	{
		// Calculate the delta function between the reference and current approximate data
		referenceDelta_.clear();
		for (int n=0; n<referenceData_.nPoints(); ++n) referenceDelta_.addPoint(referenceData_.x(n), referenceData_.y(n) - approximateData_.y(n));

		// Keep track of the number of Gaussians we add this cycle
		int nAdded = 0;

		// Go over points in the delta, calculating the gradient as we go, and seeking gradient minima (actually, crossovers between -ve and +ve gradients)
		for (int n=regionDelta; n<referenceData_.nPoints() - regionDelta; ++n)
		{
			// Calculate gradient at this point
			gradient = 0.0;
			for (int m=-regionDelta; m<regionDelta; ++m) gradient += (referenceDelta_.y(n+m+1) - referenceDelta_.y(n+m)) / (referenceDelta_.x(n+m+1) - referenceDelta_.x(n+m));

// 			Messenger::printVerbose("Gradient @ x = %f = %f\n", referenceData_.x(n), gradient);

			// Check sign of previous gradient vs the current one - do we add a Gaussian at this point?
			if ((lastSign != DissolveMath::sgn(gradient)))
			{
				trialX = referenceDelta_.x(n);
				trialA = referenceDelta_.y(n);
				trialC = 0.1;

				Messenger::printVerbose("Attempting Gaussian addition for peak/trough located at x = %f\n", trialX);

				// Set up minimiser, minimising test Gaussian only
				PrAxis<GaussFit> gaussMinimiser(*this, &GaussFit::costAmplitudeWidthXCentreStaticTrial);
				gaussMinimiser.addTarget(trialA);
				gaussMinimiser.addTarget(trialC);
				gaussMinimiser.addTarget(trialX);
				double trialError = gaussMinimiser.minimise(0.01, 0.1);

// 				// Generate trial data consisting of the current approximateData_ plus contribution from the trial Gaussian
// 				for (int m=0; m<referenceData_.nPoints(); ++m)
// 				{
// 					double x = referenceData_.x(m);
// 					trialDataY[m] = approximateData_.y(m) + trialA * exp(-((x-trialX)*(x-trialX))/(2*trialC*trialC));
// 				}
// 
// 				// Have we decreased the overall error?
// 				double trialError = referenceData_.error(trialData);

				if (trialError < currentError_)
				{
					Messenger::printVerbose("Accepting new Gaussian x = %f, A = %f, c = %f - error reduced from %f to %f\n", trialX, trialA, fabs(trialC), currentError_, trialError);
					currentError_ = trialError;

					A_.add(trialA);
					x_.add(trialX);
					c_.add(fabs(trialC));
					++nGaussians_;
					++nAdded;

					// Add the accepted Gaussian in to the approximate data, and remove it from the reference delta
					double x, y;
					for (int m=0; m<referenceData_.nPoints(); ++m)
					{
						x = referenceData_.x(m);
						y = trialA * exp(-((x-trialX)*(x-trialX))/(2*trialC*trialC));
						approximateData_.addY(m, y);
						referenceDelta_.addY(m, -y);
					}

					// Check on error / nGaussians
					if (currentError_ <= requiredError) break;
					if (nGaussians_ == maxGaussians) break;
				}
				else Messenger::printVerbose("Rejecting new Gaussian x = %f, A = %f, c = %f - error increased from %f to %f\n", trialX, trialA, fabs(trialC), currentError_, trialError);
			}

			// Store current sign of gradient
			lastSign = DissolveMath::sgn(gradient);
		}

		// Check on error
		if (currentError_ <= requiredError)
		{
			Messenger::printVerbose("Required error threshold (%f) achieved - current error is %f, nGaussians = %i\n", requiredError, currentError_, nGaussians_);
			break;
		}

		// Check on nGaussians
		if (nGaussians_ == maxGaussians)
		{
			Messenger::printVerbose("Maximum number of Gaussians (%i) reached - current error is %f\n", nGaussians_, currentError_);
			break;
		}

		// If we added no Gaussians this cycle, bail out now
		if (nAdded == 0)
		{
			Messenger::printVerbose("No Gaussians added during last cycle, so exiting now.\n");
			break;
		}
	} while (1);

	return currentError_;
}

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

	return referenceData_.error(approximateData_);
}

/*
 * Cost Function Callbacks
 */

// Return percentage error between reference data and approximate data augmented with specified Gaussian
double GaussFit::referenceError(double xCentre, double A, double C) const
{
	double sume = 0.0, sumf = 0.0, sumy = 0.0;
	const Array<double> refX = referenceData_.constArrayX();
	const Array<double> refY = referenceData_.constArrayY();
	double x, y, yApprox, c2 = C*C;
	for (int n=0; n<referenceData_.nPoints(); ++n)
	{
		// Grab reference x and y values
		x = refX.value(n);
		y = refY.value(n);

		// Calculate yApprox at this x
		yApprox = approximateData_.y(n) + A * exp(-((x-xCentre)*(x-xCentre))/(2*c2));

		// Accumulate numerator - sum of forecast errors
		sume += fabs(y - yApprox);
		sumy += fabs(yApprox);

		// Accumulate denominator - one-step naive forecast (backwards forecast for first point)
		if (n > 0) sumf += fabs(y - refY.value(n-1));
		else if (n < refX.nItems()-1) sumf += fabs(y - refY.value(n+1));
	}

	double mape = sume;
	if (sumf > 0.0) mape /= sumf;

	return mape;
}

// Two-parameter cost function over full sourceData, using current approximateData_ and alpha array containing new trial A and c values
double GaussFit::costAmplitudeWidthStaticTrial(double* alpha, int nAlpha)
{
	double A = alpha[0], c2 = alpha[1]*alpha[1], xCentre = fitX_[0];

	double sose = 0.0, x, y, dy;

	// Dumb, but effective, check for c2 becoming zero
	if (c2 == 0.0) return 100.0;

	for (int n=0; n<referenceData_.nPoints(); ++n)
	{
		x = referenceData_.x(n);

		// Calculate current y - interpolated value of approximateData_ plus contribution from trial Gaussian
		y = approximateData_.y(n) + A * exp(-((x-xCentre)*(x-xCentre))/(2*c2));

		dy = referenceData_.y(n) - y;

		sose += dy*dy;
	}

	return sose;
}

// Three-parameter cost function over full sourceData, using current approximateData_ and alpha array containing new trial x, A and c values
double GaussFit::costAmplitudeWidthXCentreStaticTrial(double* alpha, int nAlpha)
{
	double A = alpha[0], c = alpha[1], xCentre = alpha[2];

	// Must check for c approaching zero
	if (fabs(c) < 1.0e-5) return currentError_*100.0;

	// Ensure xCentre never goes outside the range of the source data
	if ((xCentre < referenceData_.xFirst()) || (xCentre > referenceData_.xLast())) return currentError_ * 100.0;

	return referenceError(xCentre, A, c);
}

// Two-parameter cost function, with alpha array containing A and c values
double GaussFit::costAmplitudeWidth(double* alpha, int nAlpha)
{
	int nGauss = nAlpha / 2;

	// Construct working array of x values
	XYData sum;
	for (int m=0; m<referenceData_.nPoints(); ++m)
	{
		if (referenceData_.x(m) < fitX_[0]) continue;
		sum.addPoint(referenceData_.x(m), 0.0);
		if (referenceData_.x(m) > fitX_[nGauss-1]) break;
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

	return referenceData_.error(sum);
}
