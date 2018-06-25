/*
	*** Gaussian Function Approximation
	*** src/math/gaussfit.h
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

#ifndef DISSOLVE_GAUSSFIT_H
#define DISSOLVE_GAUSSFIT_H

#include "templates/array.h"

// Forward Declarations
class XYData;

// Gaussian Function Approximation
class GaussFit
{
	public:
	// Constructor / Destructor
	GaussFit(const XYData& referenceData);


	/*
	 * Data
	 */
	private:
	// Reference XYData to which we are performing the fit
	XYData referenceData_;
	// Approximate (fitted) data
	XYData approximateData_;
	// Number of Gaussians used in fit
	int nGaussians_;
	// Function centres
	Array<double> x_;
	// Amplitudes
	Array<double> A_;
	// FWHM values
	Array<double> fwhm_;

	private:
	// Generate full approximation from current parameters
	void generateApproximation(bool realSpace);
	// Add contribution to specified XYData
	void addFunction(XYData& data, bool realSpace, double xCentre, double A, double fwhm) const;
	// Return value of Gaussian at specified x value
	double gaussian(double x, double xCentre, double A, double FWHM) const;
	// Return Fourier transform of Gaussian at specified x value
	double gaussianFT(double x, double xCentre, double A, double FWHM) const;

	public:
	// Return approximate function
	const XYData& approximation() const;
	// Calculate and return approximate function in requested space
	XYData approximation(bool realSpace, double factor, double xMin, double xStep, double xMax, double fwhmFactor = 1.0) const;
	// Set current parameters
	bool set(const Array<double>& x, const Array<double>& A, const Array<double>& fwhm);
	// Return number of Gaussians in fit
	int nGaussians() const;
	// Return current function centres
	const Array<double>& x() const;
	// Return current amplitudes
	const Array<double>& A() const;
	// Return current full-width half-maximum values
	const Array<double>& fwhm() const;
	// Save coefficients to specified file
	bool saveCoefficients(const char* filename) const;
	// Print coefficients
	void printCoefficients() const;
	// Save Fourier-transformed Gaussians to individual files
	bool saveFTGaussians(const char* filenamePrefix, double xStep = -1.0) const;


	/*
	 * Fitting
	 */
	private:
	// Current error
	double currentError_;
	// Whether Gaussians being fit are in real or reciprocal space
	bool alphaReal_;
	// Indices of Gaussians being fit
	Array<int> alphaIndex_;
	// Precalculated function data
	Array2D<double> functions_;

	private:
	// Update precalculated function data (using A = 1.0)
	void updatePrecalculatedFunctions();

	public:
	// Construct suitable representation in with minimal real-space Gaussians
	double constructReal(double requiredError, int maxGaussians = -1);
	// Construct function representation in reciprocal space, spacing Gaussians out evenly in real space up to rMax
	double constructReciprocal(double rMax, int nGaussians, double sigmaQ = 0.02);
	// Re-fit amplitudes in specified space, starting from current parameters
	double reFitA(bool realSpace, int sampleSize = 10, int overlap = 2, int nLoops = 3);


	/*
	 * Cost Functions
	 */
	private:
	// One-parameter cost function (amplitude) with alpha array containing A values, including current approximate data into sum
	double costAnalyticA(const Array<double>& alpha);
	// Two-parameter cost function (amplitude and FWHM) with alpha array containing A and FWHM values, including current approximate data into sum
	double costAnalyticAF(const Array<double>& alpha);
	// Two-parameter cost function (amplitude and xCentre) with alpha array containing A and FWHM values, including current approximate data into sum
	double costAnalyticAX(const Array<double>& alpha);
	// Three-parameter cost function (amplitude, FWHM, and xCentre) with alpha array containing A and FWHM values, including current approximate data into sum
	double costAnalyticAFX(const Array<double>& alpha);
	// One-parameter cost function (amplitude) using pre-calculated function array, including current approximate data in sum
	double costTabulatedA(const Array<double>& alpha);
};

#endif

