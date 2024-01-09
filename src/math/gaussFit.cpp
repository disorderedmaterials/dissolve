// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/gaussFit.h"
#include "base/lineParser.h"
#include "math/data1D.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/mc.h"
#include "math/praxis.h"
#include "templates/algorithms.h"

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
    for (auto n = 0; n < nGaussians_; ++n)
        addFunction(approximateData_, space, x_[n], A_[n], fwhm_[n]);
}

// Add contribution to specified Data1D
void GaussFit::addFunction(Data1D &data, FunctionSpace::SpaceType space, double xCentre, double A, double fwhm) const
{
    // Functional form of function to add depends on whether we're fitting Gaussians or FTs of Gaussians
    if (space == FunctionSpace::RealSpace)
    {
        for (auto m = 0; m < data.nValues(); ++m)
            data.value(m) += gaussian(data.xAxis(m), xCentre, A, fwhm);
    }
    else
    {
        for (auto m = 0; m < data.nValues(); ++m)
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

    auto c = FWHM / TWOSQRT2LN2;
    auto gfac = (sqrt(0.5 * PI) / (4.0 * PI * PI)) / c;
    if ((x > 0.0) && (xCentre > 0.0))
        gfac /= x * xCentre;
    else
        gfac *= 2.0 / (c * c);
    return gfac * A * exp(-((x - xCentre) * (x - xCentre)) / (2.0 * c * c));
}

// Return Fourier transform of Gaussian at specified x value
double GaussFit::gaussianFT(double x, double xCentre, double A, double FWHM) const
{
    auto c = FWHM / TWOSQRT2LN2;
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
const std::vector<double> &GaussFit::x() const { return x_; }

// Return current amplitudes
const std::vector<double> &GaussFit::A() const { return A_; }

// Return amplitudes (and xCentres) as Data1D
Data1D GaussFit::Ax() const
{
    Data1D data;

    std::copy(x_.begin(), x_.end(), data.xAxis().begin());
    std::copy(A_.begin(), A_.end(), data.values().begin());

    return data;
}

// Return current full-width half-maximum values
const std::vector<double> &GaussFit::fwhm() const { return fwhm_; }

// Save coefficients to specified file
bool GaussFit::saveCoefficients(std::string_view filename) const
{
    LineParser parser;
    if (!parser.openOutput(filename))
        return false;

    parser.writeLineF("#  x  A  FWHM\n");
    for (auto n = 0; n < nGaussians_; ++n)
        parser.writeLineF("{}  {}  {}\n", x_[n], A_[n], fwhm_[n]);

    parser.closeFiles();

    return true;
}

// Print coefficients
void GaussFit::printCoefficients() const
{
    Messenger::print("Fitted nGaussians = {}:\n", nGaussians_);
    Messenger::print(" Gauss     A         x         FWHM\n");
    for (auto n = 0; n < nGaussians_; ++n)
        Messenger::print("  {:4d}  =  {} {} {}\n", n, A_[n], x_[n], fwhm_[n]);
}

// Save Fourier-transformed Gaussians to individual files
bool GaussFit::saveFTGaussians(std::string_view filenamePrefix, double xStep) const
{
    double xDelta = (xStep < 0.0 ? referenceData_.xAxis(1) - referenceData_.xAxis(0) : xStep);
    for (auto n = 0; n < nGaussians_; ++n)
    {
        LineParser parser;
        if (!parser.openOutput(fmt::format("{}-{:03d}.gauss", filenamePrefix, n)))
            return false;

        auto xCentre = x_[n];
        auto A = A_[n];
        auto fwhm = fwhm_[n];
        if (!parser.writeLineF("#  x={}  A={}  fwhm={}\n", xCentre, A, fwhm))
            return false;

        double x = referenceData_.xAxis().front();
        while (x < referenceData_.xAxis().back())
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
    for (auto n = 0; n < nGaussians_; ++n)
        addFunction(approx, space, x_[n], A_[n], fwhm_[n] * fwhmFactor);

    approx *= factor;

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
    addFunction(func, space, x_[index], A_[index], fwhm_[index] * fwhmFactor);

    func *= factor;

    return func;
}

// Set coefficients from supplied values
void GaussFit::set(double rMax, const std::vector<double> &A, double sigma)
{
    // Clear any existing data
    x_.clear();
    A_.clear();
    fwhm_.clear();

    // Set new data
    nGaussians_ = A.size();
    A_ = A;

    double x, gDelta = rMax / nGaussians_;
    for (auto n = 0; n < nGaussians_; ++n)
    {
        x = (n + 1) * gDelta;
        x_.push_back(x);
        fwhm_.push_back(sigma);
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
        for (auto n = 0; n < nGaussians_; ++n)
        {
            for (auto m = 0; m < referenceData_.nValues(); ++m)
                functions_[{n, m}] = gaussian(referenceData_.xAxis(m), x_[n], A, fwhm_[n]);
        }
    }
    else
    {
        for (auto n = 0; n < nGaussians_; ++n)
        {
            for (auto m = 0; m < referenceData_.nValues(); ++m)
                functions_[{n, m}] = gaussianFT(referenceData_.xAxis(m), x_[n], A, fwhm_[n]);
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
    alphaSpace_ = space;

    for (auto loop = 0; loop < nLoops; ++loop)
    {
        // Index of the Gaussian in the x_, A_, and fwhm_ arrays is given by 'g'
        auto g = loop * (sampleSize / nLoops);
        while (g < nGaussians_)
        {

            // Generate the approximate data - we will subtract the Gaussians that we are fitting in the next loop
            generateApproximation(alphaSpace_);

            // Set up minimiser for the next batch
            MonteCarloMinimiser gaussMinimiser(
                [this]()
                {
                    double sose = 0.0;
                    double multiplier = 1.0;

                    // Loop over data points, add in our Gaussian contributions, and
                    double dy;
                    for (auto &&[x, y, refY] :
                         zip(approximateData_.xAxis(), approximateData_.values(), referenceData_.values()))
                    {
                        // Add in contributions from our Gaussians
                        for (auto n = 0; n < A_.size(); ++n)
                            y += functionValue(alphaSpace_, x, x_[n], A_[n], fwhm_[n]);

                        dy = refY - y;
                        sose += dy * dy;
                    }

                    return sose * multiplier;
                });

            // Add Gaussian parameters as fitting targets
            for (auto n = 0; n < sampleSize; ++n)
            {
                // Add the Gaussian only if its xCentre is above xMin
                if (x_[g] >= xMin)
                {
                    gaussMinimiser.addTarget(&A_[g]);

                    // Remove this Gaussian from the approximate data
                    addFunction(approximateData_, space, x_[g], -A_[g], fwhm_[g]);
                }

                // Increase Gaussian index - if that was the last one, break now
                ++g;
                if (g == nGaussians_)
                    break;
            }

            // Optimise this set of Gaussians
            gaussMinimiser.setStepSize(0.01);
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

    auto errorReport = Error::percent(referenceData_, approximateData_);
    Messenger::print(Error::errorReportString(errorReport));

    return errorReport.error;
}

// Construct function representation in reciprocal space, spacing Gaussians out evenly in real space up to rMax
double GaussFit::constructReciprocal(double rMin, double rMax, int nGaussians, double sigmaQ, int nIterations,
                                     double initialStepSize, std::optional<int> degreeOfSmoothing)
{
    return constructReciprocal(rMin, rMax, std::vector<double>(nGaussians, 0.0), sigmaQ, nIterations, initialStepSize,
                               degreeOfSmoothing);
}

// Construct function representation in reciprocal space using specified parameters as starting point
double GaussFit::constructReciprocal(double rMin, double rMax, const std::vector<double> &A, double sigmaQ, int nIterations,
                                     double initialStepSize, std::optional<int> degreeOfSmoothing)
{
    // Create the fitting functions
    A_ = A;
    x_.clear();
    fwhm_.clear();
    nGaussians_ = A_.size();
    double x, gDelta = rMax / nGaussians_;
    for (auto n = 0; n < nGaussians_; ++n)
    {
        x = (n + 1) * gDelta;
        x_.push_back(x);
        fwhm_.push_back(sigmaQ);
    }

    // Update the tabulated functions
    alphaSpace_ = FunctionSpace::ReciprocalSpace;
    updatePrecalculatedFunctions(alphaSpace_);

    // Clear the approximate data
    approximateData_.initialise(referenceData_);

    // Perform Monte Carlo minimisation on the amplitudes
    MonteCarloMinimiser gaussMinimiser(
        [this]()
        {
            auto sose = 0.0;

            // Loop over data points and sum contributions from tabulated functions on to the current approximate data
            double y, dy;
            for (auto i = 0; i < approximateData_.nValues(); ++i)
            {
                // Get approximate data x and y for this point
                y = approximateData_.value(i);

                // Add in contributions from our Gaussians
                for (auto n = 0; n < A_.size(); ++n)
                    y += functions_[{n, i}] * A_[n];

                dy = referenceData_.value(i) - y;
                sose += dy * dy;
            }

            return sose;
        },
        [degreeOfSmoothing](std::vector<double> &params)
        {
            if (degreeOfSmoothing)
                Filters::movingAverage(params, *degreeOfSmoothing);
        });

    // Add the Gaussian amplitudes to the fitting pool - ignore any whose x centre is below rMin
    for (auto n = 0; n < nGaussians_; ++n)
    {
        if (x_[n] < rMin)
            continue;
        gaussMinimiser.addTarget(&A_[n]);
    }

    // Optimise this set of Gaussians
    gaussMinimiser.setMaxIterations(nIterations);
    gaussMinimiser.setStepSize(initialStepSize);
    gaussMinimiser.setSamplingFrequency(nIterations / 2.5);
    currentError_ = gaussMinimiser.minimise();

    // Regenerate approximation and calculate percentage error of fit
    generateApproximation(FunctionSpace::ReciprocalSpace);
    currentError_ = Error::percent(referenceData_, approximateData_).error;

    return currentError_;
}
