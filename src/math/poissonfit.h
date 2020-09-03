/*
    *** Poisson Function Approximation
    *** src/math/poissonfit.h
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

// Poisson Function Approximation to Q-Space Data (replicating EPSR's methodology)
class PoissonFit
{
    public:
    PoissonFit(const Data1D &referenceData);

    /*
     * Data
     */
    private:
    // Reference Data1D to which we are performing the fit
    Data1D referenceData_;
    // Approximate (fitted) data
    Data1D approximateData_;
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
    // Broadening (decay) applied to functions in r-space
    double rBroad_;
    // Atomic density associated to reference data
    double rho_;
    // Number of functions used in fit
    int nPoissons_;
    // Function scale coefficients
    Array<double> C_;
    // Whether the first coefficient should be ignored (set to zero)
    bool ignoreZerothTerm_;
    // Maximum value of exponential
    const double expMax_;

    private:
    // Generate full approximation from current parameters
    void generateApproximation(FunctionSpace::SpaceType space);
    // Add contribution to specified Data1D
    void addFunction(Data1D &data, FunctionSpace::SpaceType space, double C, const int nIndex) const;
    // Return value of Poisson function at x value given specified power index
    double poisson(const double x, const int nIndex) const;
    // Return Fourier transform of Poisson function at Q index given specified power index
    double poissonFT(const int qIndex, const int nIndex) const;

    public:
    // Return approximate function
    const Data1D &approximation() const;
    // Calculate and return approximate function in requested space
    Data1D approximation(FunctionSpace::SpaceType space, double factor, double xMin, double xStep, double xMax) const;
    // Calculate and return single function in requested space
    Data1D singleFunction(int index, FunctionSpace::SpaceType space, double factor, double xMin, double xStep,
                          double xMax) const;
    // Set coefficients from supplied values
    void set(FunctionSpace::SpaceType space, double rMax, Array<double> coefficients, double sigmaQ = 0.02,
             double sigmaR = 0.08);
    // Return number of Poisson functions in fit
    int nPoissons() const;
    // Set whether the first coefficient should be ignored (set to zero)
    void setIgnoreZerothTerm(bool ignore);
    // Return whether the first coefficient should be ignored (set to zero)
    bool ignoreZerothTerm() const;
    // Return current C values
    const Array<double> &C() const;
    // Save coefficients to specified file
    bool saveCoefficients(std::string_view filename) const;

    /*
     * Fitting
     */
    private:
    // Current error
    double currentError_;
    // Function space in which current alpha are being fit
    FunctionSpace::SpaceType alphaSpace_;
    // Precalculated terms
    Array<double> sqrtOnePlusQSqSigmaSq_, arcTanQSigma_, oneMinusQSqSigmaSq_, lnNPlusTwoFactorial_;
    Array<int> n_;
    double fourPiSigmaRCubed_;
    // Precalculated function data
    Array2D<double> functions_;
    // Indices of Gaussians being fit
    Array<int> alphaIndex_;

    private:
    // Precalculate necessary terms
    void preCalculateTerms();
    // Update precalculated function data using specified C
    void updatePrecalculatedFunctions(FunctionSpace::SpaceType space, double C = 1.0);
    // Sweep-fit coefficients in specified space, starting from current parameters
    double sweepFitC(FunctionSpace::SpaceType space, double xMin, int sampleSize = 10, int overlap = 2, int nLoops = 3);

    public:
    // Construct suitable reciprocal-space representation using given number of Poissons spaced evenly in real space up to
    // rMax (those below rMin will be ignored)
    double constructReciprocal(double rMin, double rMax, int nPoissons, double sigmaQ = 0.02, double sigmaR = 0.08,
                               int nIterations = 1000, double initialStepSize = 0.01, int smoothingThreshold = 0,
                               int smoothingK = 3, int smoothingM = 3, bool reFitAtEnd = false);
    // Construct suitable reciprocal-space representation using provided coefficients as a starting point
    double constructReciprocal(double rMin, double rMax, Array<double> coefficients, double sigmaQ = 0.02, double sigmaR = 0.08,
                               int nIterations = 1000, double initialStepSize = 0.01, int smoothingThreshold = 0,
                               int smoothingK = 3, int smoothingM = 3, bool reFitAtEnd = false);

    /*
     * Cost Functions
     */
    private:
    // One-parameter cost function (coefficient) with alpha array containing C values, including current approximate data
    // into sum
    double costAnalyticC(const Array<double> &alpha);
    // One-parameter cost function (coefficient) using pre-calculated function array, including current approximate data in
    // sum
    double costTabulatedC(const Array<double> &alpha);
};
