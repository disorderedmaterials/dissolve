/*
    *** Gaussian Function Approximation
    *** src/math/gaussfit.h
    Copyright T. Youngs 2019-2020

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

#pragma once

#include "math/data1d.h"
#include "math/functionspace.h"
#include "templates/array.h"
#include "templates/array2d.h"

// Gaussian Function Approximation
class GaussFit
{
    public:
    GaussFit(const Data1D &referenceData);

    /*
     * Data
     */
    private:
    // Reference Data1D to which we are performing the fit
    Data1D referenceData_;
    // Approximate (fitted) data
    Data1D approximateData_;
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
    void generateApproximation(FunctionSpace::SpaceType space);
    // Add contribution to specified Data1D
    void addFunction(Data1D &data, FunctionSpace::SpaceType space, double xCentre, double A, double fwhm) const;
    // Return value of Gaussian at specified x value
    double gaussian(double x, double xCentre, double A, double FWHM) const;
    // Return Fourier transform of Gaussian at specified x value
    double gaussianFT(double x, double xCentre, double A, double FWHM) const;
    // Return function value at specified x value in desired space
    double functionValue(FunctionSpace::SpaceType space, double x, double xCentre, double A, double FWHM) const;

    public:
    // Return approximate function
    const Data1D &approximation() const;
    // Calculate and return approximate function in requested space
    Data1D approximation(FunctionSpace::SpaceType space, double factor, double xMin, double xStep, double xMax,
                         double fwhmFactor = 1.0) const;
    // Calculate and return single function in requested space
    Data1D singleFunction(int index, FunctionSpace::SpaceType space, double factor, double xMin, double xStep, double xMax,
                          double fwhmFactor = 1.0) const;
    // Set coefficients from supplied values
    void set(double rMax, const Array<double> &A, double sigma);
    // Return number of Gaussians in fit
    int nGaussians() const;
    // Return current function centres
    const Array<double> &x() const;
    // Return current amplitudes
    const Array<double> &A() const;
    // Return amplitudes (and xCentres) as Data1D
    Data1D Ax() const;
    // Return current full-width half-maximum values
    const Array<double> &fwhm() const;
    // Save coefficients to specified file
    bool saveCoefficients(std::string_view filename) const;
    // Print coefficients
    void printCoefficients() const;
    // Save Fourier-transformed Gaussians to individual files
    bool saveFTGaussians(std::string_view filenamePrefix, double xStep = -1.0) const;

    /*
     * Fitting
     */
    private:
    // Current error
    double currentError_;
    // Function space relevant to current functions being fit space
    FunctionSpace::SpaceType alphaSpace_;
    // Indices of Gaussians being fit
    Array<int> alphaIndex_;
    // Precalculated function data
    Array2D<double> functions_;

    private:
    // Update precalculated function data using specified A
    void updatePrecalculatedFunctions(FunctionSpace::SpaceType space, double A = 1.0);
    // Sweep-fit amplitudes in specified space, starting from current parameters
    double sweepFitA(FunctionSpace::SpaceType space, double xMin, int sampleSize = 10, int overlap = 2, int nLoops = 3);

    public:
    // Construct suitable representation in with minimal real-space Gaussians
    double constructReal(double requiredError, int maxGaussians = -1);
    // Construct function representation in reciprocal space, spacing Gaussians out evenly in real space up to rMax (those
    // below rMin will be excluded)
    double constructReciprocal(double rMin, double rMax, int nGaussians, double sigmaQ = 0.02, int nIterations = 1000,
                               double initialStepSize = 0.01, int smoothingThreshold = 0, int smoothingK = 3,
                               int smoothingM = 3, bool reFitAtEnd = false);
    // Construct function representation in reciprocal space using specified parameters as starting point
    double constructReciprocal(double rMin, double rMax, const Array<double> &A, double sigmaQ = 0.02, int nIterations = 1000,
                               double initialStepSize = 0.01, int smoothingThreshold = 0, int smoothingK = 3,
                               int smoothingM = 3, bool reFitAtEnd = false);

    /*
     * Cost Functions
     */
    private:
    // One-parameter cost function (amplitude) with alpha array containing A values, including current approximate data into
    // sum
    double costAnalyticA(const Array<double> &alpha);
    // Two-parameter cost function (amplitude and FWHM) with alpha array containing A and FWHM values, including current
    // approximate data into sum
    double costAnalyticAF(const Array<double> &alpha);
    // Two-parameter cost function (amplitude and xCentre) with alpha array containing A and FWHM values, including current
    // approximate data into sum
    double costAnalyticAX(const Array<double> &alpha);
    // Three-parameter cost function (amplitude, FWHM, and xCentre) with alpha array containing A and FWHM values, including
    // current approximate data into sum
    double costAnalyticAFX(const Array<double> &alpha);
    // One-parameter cost function (amplitude) using pre-calculated function array, including current approximate data in
    // sum
    double costTabulatedA(const Array<double> &alpha);
};
