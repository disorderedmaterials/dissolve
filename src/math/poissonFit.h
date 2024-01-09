// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "math/functionSpace.h"
#include "templates/array2D.h"

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
    std::vector<double> C_;
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
    void set(FunctionSpace::SpaceType space, double rMax, const std::vector<double> &coefficients, double sigmaQ = 0.02,
             double sigmaR = 0.08);
    // Return number of Poisson functions in fit
    int nPoissons() const;
    // Set whether the first coefficient should be ignored (set to zero)
    void setIgnoreZerothTerm(bool ignore);
    // Return whether the first coefficient should be ignored (set to zero)
    bool ignoreZerothTerm() const;
    // Return current C values
    const std::vector<double> &C() const;
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
    std::vector<double> sqrtOnePlusQSqSigmaSq_, arcTanQSigma_, oneMinusQSqSigmaSq_, lnNPlusTwoFactorial_;
    std::vector<int> n_;
    double fourPiSigmaRCubed_;
    // Precalculated function data
    Array2D<double> functions_;

    private:
    // Precalculate necessary terms
    void preCalculateTerms();
    // Update precalculated function data using specified C
    void updatePrecalculatedFunctions(FunctionSpace::SpaceType space, double C = 1.0);
    // Calculate sum of squares error between reference data and function represented by current parameters
    double calculateReferenceError() const;
    // Sweep-fit coefficients in specified space, starting from current parameters
    double sweepFitC(FunctionSpace::SpaceType space, double xMin, int sampleSize = 10, int overlap = 2, int nLoops = 3);

    public:
    // Construct suitable reciprocal-space representation using given number of Poissons spaced evenly in real space up to
    // rMax (those below rMin will be ignored)
    double constructReciprocal(double rMin, double rMax, int nPoissons, double sigmaQ = 0.02, double sigmaR = 0.08,
                               int nIterations = 1000, double initialStepSize = 0.01,
                               std::optional<int> degreeOfSmoothing = {});
    // Construct suitable reciprocal-space representation using provided coefficients as a starting point
    double constructReciprocal(double rMin, double rMax, const std::vector<double> &coefficients, double sigmaQ = 0.02,
                               double sigmaR = 0.08, int nIterations = 1000, double initialStepSize = 0.01,
                               std::optional<int> degreeOfSmoothing = {});
};
