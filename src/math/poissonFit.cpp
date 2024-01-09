// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/poissonFit.h"
#include "base/lineParser.h"
#include "math/data1D.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/mc.h"
#include "math/praxis.h"
#include "templates/algorithms.h"

PoissonFit::PoissonFit(const Data1D &referenceData) : expMax_(25.0)
{
    referenceData_ = referenceData;

    alphaSpace_ = FunctionSpace::RealSpace;
    rBroad_ = 0.0;
    nPoissons_ = 0;
    ignoreZerothTerm_ = true;
}

/*
 * Data
 */

// Generate full approximation from current parameters
void PoissonFit::generateApproximation(FunctionSpace::SpaceType space)
{
    approximateData_.initialise(referenceData_);

    // Sum defined functions
    for (auto n = 0; n < nPoissons_; ++n)
        addFunction(approximateData_, space, C_[n], n);
}

// Add contribution to specified Data1D
void PoissonFit::addFunction(Data1D &data, FunctionSpace::SpaceType space, double C, const int nIndex) const
{
    if (space == FunctionSpace::RealSpace)
    {
        for (auto m = 0; m < data.nValues(); ++m)
            data.value(m) += C * poisson(data.xAxis(m), nIndex);
    }
    else
    {
        // We assume here that the supplied 'data' has abscissa consistent with the precalculated data
        for (auto m = 0; m < data.nValues(); ++m)
            data.value(m) += C * poissonFT(m, nIndex);
    }
}

// Return value of Poisson function at x value given specified power index
double PoissonFit::poisson(const double x, const int nIndex) const
{
    /*
     * Functional Form [Phys. Rev. B, 72, 104204 (2005) - Eq 7]
     *
     * 			    1	        (   r   )n     (     r   )
     * p[n](r,sigma) = -------------------- ( ----- )  exp ( - ----- )
     * 		   4 pi sigma**3 (n+2)! ( sigma )      (   sigma )
     *
     * Overflows possible from large values of n, so treat everything logged and explicitly check for
     * values going over some predefined limit (expMax_).
     *
     * Note: Equation as written does not contain a factor of rho in the denominator, but such a factor
     * is present in the generating function (calc_pwrexpr) in EPSR. The factor of rho is not applied here,
     * being introduced in the process of generating the empirical potentials instead.
     */

    // Calculate natural log of denominator in prefactor
    auto lnFactor = log(fourPiSigmaRCubed_) + lnNPlusTwoFactorial_[nIndex];

    // At x == 0 only the first function (with nIndex == 0) contributes - all others are zero
    auto exponent = -(x / sigmaR_) - lnFactor - (rBroad_ * x);
    if (x > 0.0)
    {
        exponent += n_[nIndex] * log(x / sigmaR_);
    }
    else if (nIndex != 0)
        return 0.0;

    // Check magnitude of exponent for return
    // TODO This does *not* prevent a maximum value...
    return (exponent > -expMax_ ? exp(exponent) : 0.0);
}

// Return Fourier transform of Poisson function at Q index given specified power index
double PoissonFit::poissonFT(const int qIndex, const int nIndex) const
{
    /*
     * Functional Form [Phys. Rev. B, 72, 104204 (2005) - Eq 9]
     *
     * 				     1			(	       1-Q*Q*sigma*sigma	  )
     * P[n](Q,sigma) = ------------------------------------ ( 2 cos(n*a) + ----------------- sin(n*a) )
     * 		   (n+2) sqrt(1+Q*Q*sigma*sigma)**(n+4) (		     Q*sigma		  )
     *
     * where a = arctan(Q*sigma).
     */

    const auto n = n_[nIndex];

    double na = n * arcTanQSigma_[qIndex];

    auto factor = 1.0 / ((n + 2) * pow(sqrtOnePlusQSqSigmaSq_[qIndex], n + 4));

    auto value = 2.0 * cos(na) + (oneMinusQSqSigmaSq_[qIndex] / (referenceData_.xAxis(qIndex) * sigmaQ_)) * sin(na);

    return factor * value;
}

// Return approximate function
const Data1D &PoissonFit::approximation() const { return approximateData_; }

// Calculate and return approximate function in requested space
Data1D PoissonFit::approximation(FunctionSpace::SpaceType space, double factor, double xMin, double xStep, double xMax) const
{
    Data1D approx;
    auto x = xMin;
    while (x <= xMax)
    {
        approx.addPoint(x, 0.0);
        x += xStep;
    }

    // Loop over defined functions
    for (auto n = 0; n < nPoissons_; ++n)
        addFunction(approx, space, C_[n], n);

    approx *= factor;

    return approx;
}

// Calculate and return single function in requested space
Data1D PoissonFit::singleFunction(int index, FunctionSpace::SpaceType space, double factor, double xMin, double xStep,
                                  double xMax) const
{
    Data1D func;
    auto x = xMin;
    while (x <= xMax)
    {
        func.addPoint(x, 0.0);
        x += xStep;
    }

    addFunction(func, space, C_[index], index);

    func *= factor;

    return func;
}

// Set coefficients from supplied values
void PoissonFit::set(FunctionSpace::SpaceType space, double rMax, const std::vector<double> &coefficients, double sigmaQ,
                     double sigmaR)
{
    // Clear any existing data
    C_.clear();

    // Set new coefficients
    nPoissons_ = coefficients.size();
    C_ = coefficients;
    sigmaQ_ = sigmaQ;
    sigmaR_ = sigmaR;
    rMin_ = 0.0;
    rMax_ = rMax;
    rStep_ = (rMax_ - rMin_) / nPoissons_;

    // Pre-calculate the necessary terms and function data
    preCalculateTerms();
    updatePrecalculatedFunctions(space);
    generateApproximation(space);
}

// Return number of Poisson functions in fit
int PoissonFit::nPoissons() const { return nPoissons_; }

// Set whether the first coefficient should be ignored (set to zero)
void PoissonFit::setIgnoreZerothTerm(bool ignore) { ignoreZerothTerm_ = ignore; }

// Return whether the first coefficient should be ignored (set to zero)
bool PoissonFit::ignoreZerothTerm() const { return ignoreZerothTerm_; }

// Return current C values
const std::vector<double> &PoissonFit::C() const { return C_; }

// Save coefficients to specified file
bool PoissonFit::saveCoefficients(std::string_view filename) const
{
    LineParser parser;
    if (!parser.openOutput(filename))
        return false;

    parser.writeLineF("#   C\n");
    for (auto n = 0; n < nPoissons_; ++n)
        parser.writeLineF("{}\n", C_[n]);

    parser.closeFiles();

    return true;
}

/*
 * Fitting
 */

// Precalculate necessary terms
void PoissonFit::preCalculateTerms()
{
    // Factors involving Q
    const auto &Q = referenceData_.xAxis();
    sqrtOnePlusQSqSigmaSq_.clear();
    sqrtOnePlusQSqSigmaSq_.resize(referenceData_.nValues());
    oneMinusQSqSigmaSq_.clear();
    oneMinusQSqSigmaSq_.resize(referenceData_.nValues());
    arcTanQSigma_.clear();
    arcTanQSigma_.resize(referenceData_.nValues());
    for (auto n = 0; n < referenceData_.nValues(); ++n)
    {
        sqrtOnePlusQSqSigmaSq_[n] = sqrt(1.0 + Q[n] * Q[n] * sigmaQ_ * sigmaQ_);
        oneMinusQSqSigmaSq_[n] = 1.0 - Q[n] * Q[n] * sigmaQ_ * sigmaQ_;
        arcTanQSigma_[n] = atan(Q[n] * sigmaQ_);
    }

    /*
     * Poisson function powers (n)  [Eq 2.3.3]
     *
     * Values of 'n' and lnGamma are calculated to be consistent with those utilised in EPSR:
     *
     *   - Stored n, used in prefactor calculation, starts at 0
     *   - lnGamma calculated from these values + 3, to give the (n+2) values used in the equation.
     */
    n_.clear();
    lnNPlusTwoFactorial_.clear();
    auto r = rStep_;
    auto deltaN = floor(rStep_ / sigmaR_ + 0.5);
    auto n = deltaN - 1;
    for (auto i = 0; i < nPoissons_; ++i)
    {
        // Store n at this r value
        n_.push_back((sigmaR_ > 0.0) && (sigmaR_ <= r) ? n : 0);

        // Calculate gamma function for (n+2) - note that this is 'n+3' int ehc
        lnNPlusTwoFactorial_.push_back(lgamma(n_[i] + 3));

        // Increase r and n
        r += rStep_;
        n += deltaN;
    }
    fourPiSigmaRCubed_ = 4.0 * PI * sigmaR_ * sigmaR_ * sigmaR_;
}

// Update precalculated function data using specified C
void PoissonFit::updatePrecalculatedFunctions(FunctionSpace::SpaceType space, double C)
{
    functions_.initialise(nPoissons_, referenceData_.nValues());

    if (space == FunctionSpace::RealSpace)
    {
        for (auto n = 0; n < nPoissons_; ++n)
        {
            for (auto m = 0; m < referenceData_.nValues(); ++m)
                functions_[{n, m}] = C * poisson(referenceData_.xAxis(m), n);
        }
    }
    else
    {
        for (auto n = 0; n < nPoissons_; ++n)
        {
            for (auto m = 0; m < referenceData_.nValues(); ++m)
                functions_[{n, m}] = C * poissonFT(m, n);
        }
    }
}

// Calculate sum of squares error between reference data and function represented by current parameters
double PoissonFit::calculateReferenceError() const
{
    auto sose = 0.0;

    double y, dy;
    for (auto i = 0; i < approximateData_.nValues(); ++i)
    {
        // Get approximate data x and y for this point
        y = approximateData_.value(i);

        // Add in contributions from our Gaussians
        for (auto n = 0; n < C_.size(); ++n)
            y += functions_[{n, i}] * C_[n];

        dy = referenceData_.value(i) - y;
        sose += dy * dy;
    }

    return sose;
}

// Sweep-fit coefficients in specified space, starting from current parameters
double PoissonFit::sweepFitC(FunctionSpace::SpaceType space, double xMin, int sampleSize, int overlap, int nLoops)
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

    currentError_ = 1.0e9;

    for (auto loop = 0; loop < nLoops; ++loop)
    {
        // Index of the function in the C_ array is given by 'p'
        auto p = loop * (sampleSize / nLoops);
        if ((p == 0) && ignoreZerothTerm_)
            p = 1;
        while (p < nPoissons_)
        {
            // Generate the approximate data - we will subtract the functions that we are fitting in the next loop
            generateApproximation(space);

            // Set up minimiser for the next batch
            MonteCarloMinimiser poissonMinimiser(
                [this]()
                {
                    auto sose = 0.0;
                    auto multiplier = 1.0;

                    // Loop over data points, add in our Gaussian contributions, and
                    double x, y, dy;
                    for (auto i = 0; i < approximateData_.nValues(); ++i)
                    {
                        // Get approximate data x and y for this point
                        x = approximateData_.xAxis(i);
                        y = approximateData_.value(i);

                        // Add in contributions from our Gaussians
                        for (auto n = 0; n < C_.size(); ++n)
                            y += (alphaSpace_ == FunctionSpace::RealSpace ? C_[n] * poisson(x, n) : C_[n] * poissonFT(i, n));

                        dy = referenceData_.value(i) - y;
                        sose += dy * dy;
                    }

                    return sose * multiplier;
                });

            alphaSpace_ = space;

            // Set-up fitting targets
            for (auto n = 0; n < sampleSize; ++n)
            {
                // Add the Poisson coefficients to the fitting pool - ignore any whose x centre is below rMin
                if (((p + 1) * sigmaR_) >= xMin)
                {
                    poissonMinimiser.addTarget(&C_[p]);

                    // Remove this function from the approximate data
                    addFunction(approximateData_, space, -C_[p], p);
                }

                // Increase function index - if that was the last one, break now
                ++p;
                if (p == nPoissons_)
                    break;
            }

            // Optimise this set of Gaussians
            poissonMinimiser.setStepSize(0.01);
            currentError_ = poissonMinimiser.minimise();
            Messenger::printVerbose("PoissonFit::reFitC() - P = {}, error = {}\n", p, currentError_);

            // If we are not at the end of the Gaussian array, move the index backwards so the next set overlaps a
            // little with this one
            if (p < nPoissons_)
                p -= overlap;
        }
    }

    // Calculate the approximate function
    generateApproximation(space);

    return Error::percent(referenceData_, approximateData_).error;
}

// Construct suitable representation using given number of Poissons spaced evenly in real space up to rMax (those below rMin
// will be zeroed)
double PoissonFit::constructReciprocal(double rMin, double rMax, int nPoissons, double sigmaQ, double sigmaR, int nIterations,
                                       double initialStepSize, std::optional<int> degreeOfSmoothing)
{
    return constructReciprocal(rMin, rMax, std::vector<double>(nPoissons, 0.0), sigmaQ, sigmaR, nIterations, initialStepSize,
                               degreeOfSmoothing);
}

// Construct suitable reciprocal-space representation using provided coefficients as a starting point
double PoissonFit::constructReciprocal(double rMin, double rMax, const std::vector<double> &coefficients, double sigmaQ,
                                       double sigmaR, int nIterations, double initialStepSize,
                                       std::optional<int> degreeOfSmoothing)
{
    // Set up data
    nPoissons_ = coefficients.size();
    C_ = coefficients;
    sigmaQ_ = sigmaQ;
    sigmaR_ = sigmaR;
    rMin_ = 0.0;
    rMax_ = rMax;
    rStep_ = rMax_ / nPoissons_;

    // Pre-calculate the necessary terms and function data
    preCalculateTerms();
    alphaSpace_ = FunctionSpace::ReciprocalSpace;
    updatePrecalculatedFunctions(alphaSpace_);

    // Clear the approximate data
    approximateData_.initialise(referenceData_);

    // Perform Monte Carlo minimisation on the amplitudes
    MonteCarloMinimiser poissonMinimiser([this]() { return calculateReferenceError(); },
                                         [degreeOfSmoothing](std::vector<double> &params)
                                         {
                                             if (degreeOfSmoothing)
                                                 Filters::movingAverage(params, *degreeOfSmoothing);
                                         });

    // Add coefficients for minimising
    for (auto n = (ignoreZerothTerm_ ? 1 : 0); n < nPoissons_; ++n)
    {
        if (((n + 1) * sigmaR_) < rMin_)
            continue;

        poissonMinimiser.addTarget(&C_[n]);
    }

    poissonMinimiser.setMaxIterations(nIterations);
    poissonMinimiser.setStepSize(initialStepSize);
    poissonMinimiser.setSamplingFrequency(nIterations / 2.5);
    poissonMinimiser.minimise();

    // Regenerate approximation and calculate percentage error of fit
    generateApproximation(FunctionSpace::ReciprocalSpace);
    currentError_ = Error::percent(referenceData_, approximateData_).error;

    return currentError_;
}
