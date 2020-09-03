/*
    *** Gaussian Function Approximation
    *** src/math/gaussfit.cpp
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

#include "math/gaussfit.h"
#include "base/lineparser.h"
#include "math/data1d.h"
#include "math/error.h"
#include "math/mc.h"
#include "math/praxis.h"

GaussFit::GaussFit(const Data1D &referenceData)
{
    referenceData_ = referenceData;

    alphaSpace_ = FunctionSpace::RealSpace;
    nGaussians_ = 0;
}

/*
 * Data
 */

// Generate full approximation from current parameters
void GaussFit::generateApproximation(FunctionSpace::SpaceType space)
{
    approximateData_.initialise(referenceData_);

    // Sum defined Gaussians
    for (int n = 0; n < nGaussians_; ++n)
        addFunction(approximateData_, space, x_.constAt(n), A_.constAt(n), fwhm_.constAt(n));
}

// Add contribution to specified Data1D
void GaussFit::addFunction(Data1D &data, FunctionSpace::SpaceType space, double xCentre, double A, double fwhm) const
{
    // Functional form of function to add depends on whether we're fitting Gaussians or FTs of Gaussians
    if (space == FunctionSpace::RealSpace)
    {
        for (int m = 0; m < data.nValues(); ++m)
            data.value(m) += gaussian(data.xAxis(m), xCentre, A, fwhm);
    }
    else
    {
        for (int m = 0; m < data.nValues(); ++m)
            data.value(m) += gaussianFT(data.xAxis(m), xCentre, A, fwhm);
    }
}

// Return value of Gaussian at specified x value
double GaussFit::gaussian(double x, double xCentre, double A, double FWHM) const
{
    /*
     * Normalisation term omits factor of rho in the denominator, and which should be applied to coefficients or the
     * preFactor argument of approximation().
     */

    double c = FWHM / TWOSQRT2LN2;
    double gfac = (sqrt(0.5 * PI) / (4.0 * PI * PI)) / c;
    if ((x > 0.0) && (xCentre > 0.0))
        gfac /= x * xCentre;
    else
        gfac *= 2.0 / (c * c);
    return gfac * A * exp(-((x - xCentre) * (x - xCentre)) / (2.0 * c * c));
}

// Return Fourier transform of Gaussian at specified x value
double GaussFit::gaussianFT(double x, double xCentre, double A, double FWHM) const
{
    double c = FWHM / TWOSQRT2LN2;
    const auto xCx = xCentre * x;
    return xCx > 0.0 ? A * exp(-(x * x * c * c) / 2.0) * sin(xCx) / (xCx) : A * exp(-(x * x * c * c) / 2.0);
}

// Return function value at specified x value in desired space
double GaussFit::functionValue(FunctionSpace::SpaceType space, double x, double xCentre, double A, double FWHM) const
{
    return (space == FunctionSpace::RealSpace ? gaussian(x, xCentre, A, FWHM) : gaussianFT(x, xCentre, A, FWHM));
}

// Return number of Gaussians in fit
int GaussFit::nGaussians() const { return nGaussians_; }

// Return current function centres
const Array<double> &GaussFit::x() const { return x_; }

// Return current amplitudes
const Array<double> &GaussFit::A() const { return A_; }

// Return amplitudes (and xCentres) as Data1D
Data1D GaussFit::Ax() const
{
    Data1D data;

    data.xAxis() = x_;
    data.values() = A_;

    return data;
}

// Return current full-width half-maximum values
const Array<double> &GaussFit::fwhm() const { return fwhm_; }

// Save coefficients to specified file
bool GaussFit::saveCoefficients(std::string_view filename) const
{
    LineParser parser;
    if (!parser.openOutput(filename))
        return false;

    parser.writeLineF("#  x  A  FWHM\n");
    for (int n = 0; n < nGaussians_; ++n)
        parser.writeLineF("{}  {}  {}\n", x_.constAt(n), A_.constAt(n), fwhm_.constAt(n));

    parser.closeFiles();

    return true;
}

// Print coefficients
void GaussFit::printCoefficients() const
{
    Messenger::print("Fitted nGaussians = {}:\n", nGaussians_);
    Messenger::print(" Gauss     A         x         FWHM\n");
    for (int n = 0; n < nGaussians_; ++n)
        Messenger::print("  {:4d}  =  {} {} {}\n", n, A_.constAt(n), x_.constAt(n), fwhm_.constAt(n));
}

// Save Fourier-transformed Gaussians to individual files
bool GaussFit::saveFTGaussians(std::string_view filenamePrefix, double xStep) const
{
    double xDelta = (xStep < 0.0 ? referenceData_.constXAxis(1) - referenceData_.constXAxis(0) : xStep);
    for (int n = 0; n < nGaussians_; ++n)
    {
        LineParser parser;
        if (!parser.openOutput(fmt::format("{}-{:03d}.gauss", filenamePrefix, n)))
            return false;

        double xCentre = x_.constAt(n);
        double A = A_.constAt(n);
        double fwhm = fwhm_.constAt(n);
        if (!parser.writeLineF("#  x={}  A={}  fwhm={}\n", xCentre, A, fwhm))
            return false;

        double x = referenceData_.constXAxis().firstValue();
        while (x < referenceData_.constXAxis().lastValue())
        {
            parser.writeLineF("{}  {}\n", x, gaussianFT(x, xCentre, A, fwhm));
            x += xDelta;
        }

        parser.closeFiles();
    }

    return true;
}

// Return approximate function
const Data1D &GaussFit::approximation() const { return approximateData_; }

// Calculate and return approximate function in requested space
Data1D GaussFit::approximation(FunctionSpace::SpaceType space, double factor, double xMin, double xStep, double xMax,
                               double fwhmFactor) const
{
    Data1D approx;
    double x = xMin;
    while (x <= xMax)
    {
        approx.addPoint(x, 0.0);
        x += xStep;
    }

    // Loop over defined Gaussians
    for (int n = 0; n < nGaussians_; ++n)
        addFunction(approx, space, x_.constAt(n), A_.constAt(n), fwhm_.constAt(n) * fwhmFactor);

    approx.values() *= factor;

    return approx;
}

// Calculate and return single function in requested space
Data1D GaussFit::singleFunction(int index, FunctionSpace::SpaceType space, double factor, double xMin, double xStep,
                                double xMax, double fwhmFactor) const
{
    Data1D func;
    double x = xMin;
    while (x <= xMax)
    {
        func.addPoint(x, 0.0);
        x += xStep;
    }

    // Loop over defined Gaussians
    addFunction(func, space, x_.constAt(index), A_.constAt(index), fwhm_.constAt(index) * fwhmFactor);

    func.values() *= factor;

    return func;
}

// Set coefficients from supplied values
void GaussFit::set(double rMax, const Array<double> &A, double sigma)
{
    // Clear any existing data
    x_.clear();
    A_.clear();
    fwhm_.clear();

    // Set new data
    nGaussians_ = A.nItems();
    A_ = A;

    double x, gDelta = rMax / nGaussians_;
    for (int n = 0; n < nGaussians_; ++n)
    {
        x = (n + 1) * gDelta;
        x_.add(x);
        fwhm_.add(sigma);
    }
}

/*
 * Fitting
 */

// Update precalculated function data using specified A
void GaussFit::updatePrecalculatedFunctions(FunctionSpace::SpaceType space, double A)
{
    functions_.initialise(nGaussians_, referenceData_.nValues());

    if (space == FunctionSpace::RealSpace)
    {
        for (int n = 0; n < nGaussians_; ++n)
        {
            for (int m = 0; m < referenceData_.nValues(); ++m)
                functions_.at(n, m) = gaussian(referenceData_.xAxis(m), x_[n], A, fwhm_[n]);
        }
    }
    else
    {
        for (int n = 0; n < nGaussians_; ++n)
        {
            for (int m = 0; m < referenceData_.nValues(); ++m)
                functions_.at(n, m) = gaussianFT(referenceData_.xAxis(m), x_[n], A, fwhm_[n]);
        }
    }
}
// Sweep-fit amplitudes in specified space, starting from current parameters
double GaussFit::sweepFitA(FunctionSpace::SpaceType space, double xMin, int sampleSize, int overlap, int nLoops)
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

    for (int loop = 0; loop < nLoops; ++loop)
    {
        // Index of the Gaussian in the x_, A_, and fwhm_ arrays is given by 'g'
        auto g = loop * (sampleSize / nLoops);
        while (g < nGaussians_)
        {
            // Generate the approximate data - we will subtract the Gaussians that we are fitting in the next loop
            generateApproximation(space);

            // Clear the reference array of Gaussian indices
            alphaIndex_.clear();

            // Set up minimiser for the next batch
            MonteCarloMinimiser<GaussFit> gaussMinimiser(*this, &GaussFit::costAnalyticA);
            gaussMinimiser.setMaxIterations(100);
            gaussMinimiser.setStepSize(0.01);
            alphaSpace_ = space;

            // Add Gaussian parameters as fitting targets
            for (int n = 0; n < sampleSize; ++n)
            {
                // Add the Gaussian only if its xCentre is above xMin
                if (x_[g] >= xMin)
                {
                    gaussMinimiser.addTarget(A_[g]);
                    alphaIndex_.add(g);

                    // Remove this Gaussian from the approximate data
                    addFunction(approximateData_, space, x_[g], -A_[g], fwhm_[g]);
                }

                // Increase Gaussian index - if that was the last one, break now
                ++g;
                if (g == nGaussians_)
                    break;
            }

            // Optimise this set of Gaussians
            currentError_ = gaussMinimiser.minimise();
            Messenger::printVerbose("GaussFit::reFitA() - G = {}, error = {}\n", g, currentError_);

            // If we are not at the end of the Gaussian array, move the index backwards so the next set overlaps a
            // little with this one
            if (g < nGaussians_)
                g -= overlap;
        }
    }

    // Calculate the approximate function
    generateApproximation(space);

    return Error::percent(referenceData_, approximateData_);
}

// Construct suitable representation with minimal Gaussians automatically
double GaussFit::constructReal(double requiredError, int maxGaussians)
{
    // Clear any existing data
    x_.clear();
    A_.clear();
    fwhm_.clear();
    nGaussians_ = 0;
    approximateData_.initialise(referenceData_);

    const auto regionWidth = 5, regionDelta = regionWidth / 2;
    auto lastSign = 0;
    double gradient, trialX, trialA, trialFWHM, lastX;
    Data1D referenceDelta;

    // Set starting error
    currentError_ = 100.0;

    // Outer loop
    while ((nGaussians_ < maxGaussians) || (maxGaussians == -1))
    {
        // Calculate the delta function between the reference and current approximate data
        referenceDelta.clear();
        for (int n = 0; n < referenceData_.nValues(); ++n)
            referenceDelta.addPoint(referenceData_.xAxis(n), referenceData_.value(n) - approximateData_.value(n));

        // Keep track of the number of Gaussians we add this cycle
        auto nAdded = 0;
        lastX = -1.0;

        // Go over points in the delta, calculating the gradient as we go, and seeking gradient minima (actually,
        // crossovers between -ve and +ve gradients)
        for (int n = regionDelta; n < referenceData_.nValues() - regionDelta; ++n)
        {
            // Calculate gradient at this point
            gradient = 0.0;
            for (int m = -regionDelta; m < regionDelta; ++m)
                gradient += (referenceDelta.value(n + m + 1) - referenceDelta.value(n + m)) /
                            (referenceDelta.xAxis(n + m + 1) - referenceDelta.xAxis(n + m));

            // If the x value of the current point is less than the last accepted Gaussian in this cycle, don't
            // attempt any function addition
            if (referenceDelta.xAxis(n) < lastX)
                continue;

            // Check sign of previous gradient vs the current one - do we add a Gaussian at this point?
            if ((lastSign != DissolveMath::sgn(gradient)))
            {
                trialX = referenceDelta.xAxis(n);
                trialA = referenceDelta.value(n);
                trialFWHM = 0.25;

                Messenger::printVerbose("Attempting Gaussian addition for peak/trough located at x = {}\n", trialX);

                // Set up minimiser, minimising test Gaussian only
                PrAxisMinimiser<GaussFit> gaussMinimiser(*this, &GaussFit::costAnalyticAFX);
                gaussMinimiser.setMaxStep(0.1);
                gaussMinimiser.setTolerance(0.01);
                gaussMinimiser.addTarget(trialA);
                gaussMinimiser.addTarget(trialFWHM);
                gaussMinimiser.addTarget(trialX);
                alphaSpace_ = FunctionSpace::RealSpace;
                double trialError = gaussMinimiser.minimise();

                // Sanity check fitted parameters before we (potentially) accept the new function
                if (fabs(trialA) < 1.0e-4)
                {
                    Messenger::printVerbose("Rejecting new Gaussian x = {}, A = {}, FWHM = {} - amplitude is too small\n",
                                            trialX, trialA, fabs(trialFWHM));
                }
                else if (fabs(trialFWHM) < 1.0e-2)
                {
                    Messenger::printVerbose("Rejecting new Gaussian x = {}, A = {}, FWHM = {} - FWHM is too small\n", trialX,
                                            trialA, fabs(trialFWHM));
                }
                else if ((trialError < currentError_) || (nGaussians_ == 0))
                {
                    if (nGaussians_ == 0)
                        Messenger::printVerbose("Accepting first Gaussian at x = {}, A = {}, FWHM = {} - error is {}\n", trialX,
                                                trialA, fabs(trialFWHM), trialError);
                    else
                        Messenger::printVerbose("Accepting new Gaussian x = {}, A = {}, FWHM = {} - "
                                                "error reduced from {} to {}\n",
                                                trialX, trialA, fabs(trialFWHM), currentError_, trialError);
                    currentError_ = trialError;

                    A_.add(trialA);
                    x_.add(trialX);
                    fwhm_.add(fabs(trialFWHM));
                    ++nGaussians_;
                    ++nAdded;
                    lastX = trialX;

                    // Add the accepted Gaussian in to the approximate data, and remove it from the
                    // reference delta
                    double x, y;
                    for (int m = 0; m < referenceData_.nValues(); ++m)
                    {
                        x = referenceData_.xAxis(m);
                        y = gaussian(x, trialX, trialA, trialFWHM);
                        approximateData_.value(m) += y;
                        referenceDelta.value(m) -= y;
                    }

                    // Check on error / nGaussians
                    if (currentError_ <= requiredError)
                        break;
                    if (nGaussians_ == maxGaussians)
                        break;
                }
                else
                    Messenger::printVerbose("Rejecting new Gaussian x = {}, A = {}, FWHM = {} - error "
                                            "increased from {} to {}\n",
                                            trialX, trialA, fabs(trialFWHM), currentError_, trialError);
            }

            // Store current sign of gradient
            lastSign = DissolveMath::sgn(gradient);

            // Check to see if we have reached the error threshold already
            if (currentError_ <= requiredError)
                break;
        }

        // Check on error
        if (currentError_ <= requiredError)
        {
            Messenger::printVerbose("Required error threshold ({}) achieved - current error is {}, nGaussians = {}\n",
                                    requiredError, currentError_, nGaussians_);
            break;
        }

        // Check on nGaussians
        if (nGaussians_ == maxGaussians)
        {
            Messenger::printVerbose("Maximum number of Gaussians ({}) reached - current error is {}\n", nGaussians_,
                                    currentError_);
            break;
        }

        // If we added no Gaussians this cycle, bail out now
        if (nAdded == 0)
        {
            Messenger::printVerbose("No Gaussians added during last cycle, so exiting now.\n");
            break;
        }
    }

    return currentError_;
}

// Construct function representation in reciprocal space, spacing Gaussians out evenly in real space up to rMax (those below
// rMin will be ignored)
double GaussFit::constructReciprocal(double rMin, double rMax, int nGaussians, double sigmaQ, int nIterations,
                                     double initialStepSize, int smoothingThreshold, int smoothingK, int smoothingM,
                                     bool reFitAtEnd)
{
    // Clear any existing data
    x_.clear();
    A_.clear();
    fwhm_.clear();
    nGaussians_ = nGaussians;
    approximateData_.initialise(referenceData_);

    double x, gDelta = rMax / nGaussians_;
    for (int n = 0; n < nGaussians_; ++n)
    {
        x = (n + 1) * gDelta;
        x_.add(x);
        A_.add(0.0);
        fwhm_.add(sigmaQ);
    }

    // Update the tabulated functions
    updatePrecalculatedFunctions(FunctionSpace::ReciprocalSpace);

    // Perform Monte Carlo minimisation on the amplitudes
    MonteCarloMinimiser<GaussFit> gaussMinimiser(*this, &GaussFit::costTabulatedA);
    gaussMinimiser.setMaxIterations(nIterations);
    gaussMinimiser.setStepSize(initialStepSize);
    gaussMinimiser.enableParameterSmoothing(smoothingThreshold, smoothingK, smoothingM);
    alphaSpace_ = FunctionSpace::ReciprocalSpace;

    // Add the Gaussian amplitudes to the fitting pool - ignore any whose x centre is below rMin
    for (int n = 0; n < nGaussians_; ++n)
    {
        if (x_[n] < rMin)
            continue;
        alphaIndex_.add(n);
        gaussMinimiser.addTarget(A_[n]);
    }

    // Optimise this set of Gaussians
    currentError_ = gaussMinimiser.minimise();

    // Perform a final grouped refit of the amplitudes
    if (reFitAtEnd)
        sweepFitA(FunctionSpace::ReciprocalSpace, rMin);

    // Regenerate approximation and calculate percentage error of fit
    generateApproximation(FunctionSpace::ReciprocalSpace);
    currentError_ = Error::percent(referenceData_, approximateData_);

    return currentError_;
}

// Construct function representation in reciprocal space using specified parameters as starting point
double GaussFit::constructReciprocal(double rMin, double rMax, const Array<double> &A, double sigmaQ, int nIterations,
                                     double initialStepSize, int smoothingThreshold, int smoothingK, int smoothingM,
                                     bool reFitAtEnd)
{
    // Create the fitting functions
    A_ = A;
    x_.clear();
    fwhm_.clear();
    nGaussians_ = A_.nItems();
    approximateData_.initialise(referenceData_);
    double x, gDelta = rMax / nGaussians_;
    for (int n = 0; n < nGaussians_; ++n)
    {
        x = (n + 1) * gDelta;
        x_.add(x);
        fwhm_.add(sigmaQ);
    }

    // Update the tabulated functions
    updatePrecalculatedFunctions(FunctionSpace::ReciprocalSpace);

    // Perform Monte Carlo minimisation on the amplitudes
    MonteCarloMinimiser<GaussFit> gaussMinimiser(*this, &GaussFit::costTabulatedA);
    gaussMinimiser.setMaxIterations(nIterations);
    gaussMinimiser.setStepSize(initialStepSize);
    gaussMinimiser.enableParameterSmoothing(smoothingThreshold, smoothingK, smoothingM);
    alphaSpace_ = FunctionSpace::ReciprocalSpace;

    // Add the Gaussian amplitudes to the fitting pool - ignore any whose x centre is below rMin
    for (int n = 0; n < nGaussians_; ++n)
    {
        if (x_[n] < rMin)
            continue;
        alphaIndex_.add(n);
        gaussMinimiser.addTarget(A_[n]);
    }

    // Optimise this set of Gaussians
    currentError_ = gaussMinimiser.minimise();

    // Perform a final grouped refit of the amplitudes
    if (reFitAtEnd)
        sweepFitA(FunctionSpace::ReciprocalSpace, rMin);

    // Regenerate approximation and calculate percentage error of fit
    generateApproximation(FunctionSpace::ReciprocalSpace);
    currentError_ = Error::percent(referenceData_, approximateData_);

    return currentError_;
}

/*
 * Cost Function Callbacks
 */

// One-parameter cost function (amplitude) with alpha array containing A values, including current approximate data into sum
double GaussFit::costAnalyticA(const Array<double> &alpha)
{
    double sose = 0.0;
    double multiplier = 1.0;

    double A;
    int g;

    // Loop over data points, add in our Gaussian contributions, and
    double x, y, dy;
    for (int i = 0; i < approximateData_.nValues(); ++i)
    {
        // Get approximate data x and y for this point
        x = approximateData_.xAxis(i);
        y = approximateData_.value(i);

        // Add in contributions from our Gaussians
        for (int n = 0; n < alpha.nItems(); ++n)
        {
            g = alphaIndex_[n];
            A = alpha.constAt(n);

            y += functionValue(alphaSpace_, x, x_[g], A, fwhm_[g]);
        }

        dy = referenceData_.value(i) - y;
        sose += dy * dy;
    }

    return sose * multiplier;
}

// Two-parameter cost function (amplitude and FWHM) with alpha array containing A and FWHM values, including current approximate
// data into sum
double GaussFit::costAnalyticAF(const Array<double> &alpha)
{
    const auto nGauss = alpha.nItems() / 2;

    double sose = 0.0;
    double multiplier = 1.0;

    double A, fwhm, xCentre;

    // Loop over data points, add in our Gaussian contributions, and
    double x, y, dy;
    for (int i = 0; i < approximateData_.nValues(); ++i)
    {
        // Get approximate data x and y for this point
        x = approximateData_.xAxis(i);
        y = approximateData_.value(i);

        // Add in contributions from our Gaussians
        for (int n = 0; n < nGauss; ++n)
        {
            xCentre = x_[alphaIndex_[n]];
            A = alpha.constAt(n * 2);
            fwhm = alpha.constAt(n * 2 + 1);

            // Must check for FWHM approaching zero and penalise accordingly
            if (fabs(fwhm) < 1.0e-5)
                ++multiplier;

            y += functionValue(alphaSpace_, x, xCentre, A, fwhm);
        }

        dy = referenceData_.value(i) - y;
        sose += dy * dy;
    }

    return sose * multiplier;
}

// Two-parameter cost function (amplitude and xCentre) with alpha array containing A and FWHM values, including current
// approximate data into sum
double GaussFit::costAnalyticAX(const Array<double> &alpha)
{
    const auto nGauss = alpha.nItems() / 2;

    double sose = 0.0;
    double multiplier = 1.0;

    double A, fwhm, xCentre;

    // Loop over data points, add in our Gaussian contributions, and
    double x, y, dy;
    for (int i = 0; i < approximateData_.nValues(); ++i)
    {
        // Get approximate data x and y for this point
        x = approximateData_.xAxis(i);
        y = approximateData_.value(i);

        // Add in contributions from our Gaussians
        for (int n = 0; n < nGauss; ++n)
        {
            A = alpha.constAt(n * 2);
            xCentre = alpha.constAt(n * 2 + 1);
            fwhm = fwhm_[alphaIndex_[n]];

            // Must check for FWHM approaching zero and penalise accordingly
            if (fabs(fwhm) < 1.0e-5)
                ++multiplier;

            y += functionValue(alphaSpace_, x, xCentre, A, fwhm);
        }

        dy = referenceData_.value(i) - y;
        sose += dy * dy;
    }

    return sose * multiplier;
}

// Three-parameter cost function (amplitude, FWHM, and xCentre) with alpha array containing A and FWHM values, including current
// approximate data into sum
double GaussFit::costAnalyticAFX(const Array<double> &alpha)
{
    const auto nGauss = alpha.nItems() / 2;

    double sose = 0.0;
    double multiplier = 1.0;

    double A, fwhm, xCentre;

    // Loop over data points, add in our Gaussian contributions, and
    double x, y, dy;
    for (int i = 0; i < approximateData_.nValues(); ++i)
    {
        // Get approximate data x and y for this point
        x = approximateData_.xAxis(i);
        y = approximateData_.value(i);

        // Add in contributions from our Gaussians
        for (int n = 0; n < nGauss; ++n)
        {
            A = alpha.constAt(n * 2);
            fwhm = alpha.constAt(n * 2 + 1);
            xCentre = alpha.constAt(n * 2 + 2);

            // Must check for FWHM approaching zero and penalise accordingly
            if (fabs(fwhm) < 1.0e-5)
                ++multiplier;

            y += functionValue(alphaSpace_, x, xCentre, A, fwhm);
        }

        dy = referenceData_.value(i) - y;
        sose += dy * dy;
    }

    return sose * multiplier;
}

// One-parameter cost function (amplitude) using pre-calculated function array, including current approximate data in sum
double GaussFit::costTabulatedA(const Array<double> &alpha)
{
    double sose = 0.0;

    // Loop over data points and sum contributions from tabulated functions on to the current approximate data
    double y, dy;
    auto nAlpha = alpha.nItems();
    for (int i = 0; i < approximateData_.nValues(); ++i)
    {
        // Get approximate data x and y for this point
        y = approximateData_.value(i);

        // Add in contributions from our Gaussians
        for (int n = 0; n < nAlpha; ++n)
            y += functions_.at(alphaIndex_[n], i) * alpha.constAt(n);

        dy = referenceData_.value(i) - y;
        sose += dy * dy;
    }

    return sose;
}
