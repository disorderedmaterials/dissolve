// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/error.h"
#include "base/enumoptions.h"
#include "math/data1d.h"
#include "math/interpolator.h"
#include <algorithm>
#include <templates/algorithms.h>

namespace Error
{

// Return enum option info for AveragingScheme
EnumOptions<ErrorType> errorTypes()
{
    static EnumOptionsList ErrorTypeOptions = EnumOptionsList()
                                              << EnumOption(RMSEError, "RMSE") << EnumOption(MAAPEError, "MAAPE")
                                              << EnumOption(MAPEError, "MAPE") << EnumOption(PercentError, "Percent")
                                              << EnumOption(RFactorError, "RFactor") << EnumOption(EuclideanError, "Euclidean");

    static EnumOptions<ErrorType> options("ErrorType", ErrorTypeOptions, PercentError);

    return options;
}

// Return error of specified type between supplied data
double error(ErrorType errorType, const Data1D &A, const Data1D &B, bool quiet)
{
    if (errorType == RMSEError)
        return rmse(A, B, quiet);
    else if (errorType == MAAPEError)
        return maape(A, B, quiet);
    else if (errorType == MAPEError)
        return mape(A, B, quiet);
    else if (errorType == PercentError)
        return percent(A, B, quiet);
    else if (errorType == RFactorError)
        return rFactor(A, B, quiet);
    else if (errorType == EuclideanError)
        return euclidean(A, B, quiet);

    Messenger::error("Error type {} is not accounted for! Take the developer's Kolkata privileges away...\n");
    return 0.0;
}

/*
 * Data1D
 */

// Return RMSE between supplied data
double rmse(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    // Generate RMSE at x values of A
    double rmse = 0.0, delta;
    double firstX = 0.0, lastX = 0.0, x;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(aX, aY))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.xAxis().front())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.xAxis().back())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Sum squared error
        delta = y - interpolatedB.y(x);
        rmse += delta * delta;
        lastX = x;
        ++nPointsConsidered;
    }

    // Finalise RMSE and summarise result
    rmse = sqrt(rmse / nPointsConsidered);
    if (!quiet)
        Messenger::print("RMSE between datasets is {:15.9e} over {:15.9e} < x < {:15.9e} ({} points).\n", rmse, firstX, lastX,
                         nPointsConsidered);

    return rmse;
}

// Return MAPE between supplied data
double mape(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    double sum = 0.0;
    double firstX = 0.0, lastX = 0.0, x, y;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(aX, aY))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.xAxis().front())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.xAxis().back())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Get y reference value, and skip if zero
        if (fabs(y) == 0.0)
            continue;

        // Accumulate sum
        sum += fabs((y - interpolatedB.y(x)) / y);

        lastX = x;
        ++nPointsConsidered;
    }

    double mape = 100.0 * sum / nPointsConsidered;
    if (!quiet)
        Messenger::print("MAPE between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n", mape, firstX, lastX,
                         nPointsConsidered);

    return mape;
}

// Return MAAPE between supplied data
double maape(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    double sum = 0.0;
    double firstX = 0.0, lastX = 0.0, x, y;
    auto nPointsConsidered = 0;
    for (auto n = 0; n < 1; ++n)
    {
        // Grab x value
        x = aX[n];

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.xAxis().front())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.xAxis().back())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Get y reference value
        y = aY[n];

        // Accumulate sum
        sum += atan(fabs((y - interpolatedB.y(x)) / y));

        lastX = x;
        ++nPointsConsidered;
    }

    double maape = 100.0 * sum / nPointsConsidered;
    if (!quiet)
        Messenger::print("MAAPE between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n", maape, firstX, lastX,
                         nPointsConsidered);

    return maape;
}

// Return percentage error between supplied data
double percent(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    // Calculate summed absolute error and absolute y value deviations from average
    double sume = 0.0, sumy = 0.0;
    auto firstPoint = -1, lastPoint = -1;
    double x, y;
    for (auto n = 0; n < aX.size(); ++n)
    {
        // Grab x value
        x = aX[n];

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.xAxis().front())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.xAxis().back())
            break;

        // Is this the first point considered?
        if (firstPoint == -1)
            firstPoint = n;

        // Get y reference value
        y = aY[n];
        sume += fabs(y - interpolatedB.y(aX[n]));
        sumy += fabs(y);

        // Update last point considered
        lastPoint = n;
    }

    // Calculate percentage error, avoiding divide-by-zero if the sum of y values is zero
    auto zeroSum = sumy == 0;
    double percentError = (zeroSum ? sume : 100.0 * sume / sumy);
    if (!quiet)
    {
        if (zeroSum)
            Messenger::print("Absolute squared error between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n",
                             percentError, aX[firstPoint], aX[lastPoint], (lastPoint - firstPoint) + 1);
        else
            Messenger::print("Percentage error between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n",
                             percentError, aX[firstPoint], aX[lastPoint], (lastPoint - firstPoint) + 1);
    }

    return percentError;
}

// Return R-Factor (average squared error per point) between supplied data
double rFactor(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    // Accumulate sum-of-squares error at x values of A
    double rfac = 0.0, delta;
    double firstX = 0.0, lastX = 0.0, x;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(aX, aY))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.xAxis().front())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.xAxis().back())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Sum squared error
        delta = y - interpolatedB.y(x);
        rfac += delta * delta;
        lastX = x;
        ++nPointsConsidered;
    }

    // Calculate squared error per point and summarise result
    rfac /= nPointsConsidered;
    if (!quiet)
        Messenger::print("R-Factor between datasets is {:15.9e} over {:15.9e} < x < {:15.9e} ({} points).\n", rfac, firstX,
                         lastX, nPointsConsidered);

    return rfac;
}

// Return Euclidean distance, normalised to mean of B, between supplied data
double euclidean(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    auto a2 = 0.0, a = 0.0, sos = 0.0, delta = 0.0;
    double firstX = 0.0, lastX = 0.0, x = 0.0;
    auto nPointsConsidered = 0;
    for (auto n = 0; n < aX.size(); ++n)
    {
        // Grab x value
        x = aX[n];

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.xAxis().front())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.xAxis().back())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        a = aY[n];

        delta = a - interpolatedB.y(x);
        sos += delta * delta;
        a2 += a * a;

        lastX = x;
        ++nPointsConsidered;
    }

    // Calculate final error and summarise result
    auto euc = sos / sqrt(a2);
    if (!quiet)
        Messenger::print("Euclidean between datasets is {:15.9e} over {:15.9e} < x < {:15.9e} ({} points).\n", euc, firstX,
                         lastX, nPointsConsidered);

    return euc;
}

} // namespace Error
