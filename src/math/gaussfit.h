/*
	*** Gaussian Function Approximation
	*** src/math/gaussfit.h
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

#ifndef DUQ_GAUSSFIT_H
#define DUQ_GAUSSFIT_H

#include "templates/array.h"

// Forward Declarations
class XYData;

// Gaussian Function Approximation
class GaussFit
{
	public:
	// Constructor / Destructor
	GaussFit(XYData& sourceData);


	/*
	 * Data
	 */
	private:
	// Source XYData upon which we are performing the fit
	const XYData& sourceData_;
	// Resulting approximate data
	XYData approximateData_;
	// Number of Gaussians used in fit
	int nGaussians_;
	// Function centres
	Array<double> x_;
	// Amplitudes
	Array<double> A_;
	// C values ( = FWHM / (2 * sqrt(2 log 2))
	Array<double> c_;

	private:
	// Generate full data and FT from current parameters, ignoring all Gaussians whose width coefficient is below the provided value
	void generateData(double cMin = -1.0);

	public:
	// Perform initial fit to source data, specifying number of Gaussians to use uniformly across source data range
	double initialise(int nGaussians, double fwhm);
	// Perform initial fit to source data, specifying spacing between Gaussians
	double initialise(double gaussianSpacing, double fwhm);
	// Construct suitable representation with minimal Gaussians automatically
	double construct(double requiredError, int maxGaussians = -1);
	// Return approximate function
	const XYData& approximation() const;
	// Return Fourier transform of approximate function, ignoring all Gaussians whose width coefficient is below the provided value
	XYData fourierTransform(double xMin, double xStep, double xMax, double cMin = -1.0) const;
	// Set current parameters
	bool set(const Array<double>& x, const Array<double>& A, const Array<double>& c);
	// Return number of Gaussians in fit
	int nGaussians() const;
	// Return current function centres
	const Array<double>& x() const;
	// Return current amplitudes
	const Array<double>& A() const;
	// Return current C values
	const Array<double>& c() const;


	/*
	 * Fitting
	 */
	private:
	// Working array for cost function
	XYData partialSum_;
	// Centre coordinates of Gaussians currently being fit
	Array<double> fitX_;

	public:
	// Re-fit to source data, starting from current parameters
	double reFit();


	/*
	 * Cost Functions
	 */
	private:
	// Two-parameter cost function over full sourceData, using current approximateData_ and alpha array containing new trial A and c values
	double costAmplitudeWidthStaticTrial(double* alpha, int nAlpha);
	// Three-parameter cost function over full sourceData, using current approximateData_ and alpha array containing new trial x, A and c values
	double costAmplitudeWidthXCentreStaticTrial(double* alpha, int nAlpha);
	// Two-parameter cost function, with alpha array containing A and c values
	double costAmplitudeWidth(double* alpha, int nAlpha);
};

#endif

