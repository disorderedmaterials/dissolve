// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/error.h"
#include "base/enumOptions.h"
#include "math/data1D.h"
#include "math/interpolator.h"
#include <algorithm>
#include <templates/algorithms.h>

namespace Error
{

// Return enum option info for AveragingScheme
EnumOptions<ErrorType> errorTypes()
{
    return EnumOptions<ErrorType>("ErrorType", {{RMSEError, "RMSE"},
                                                {MAAPEError, "MAAPE"},
                                                {MAPEError, "MAPE"},
                                                {PercentError, "Percent"},
                                                {RFactorError, "RFactor"},
                                                {EuclideanError, "Euclidean"}});
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

    throw(
        std::runtime_error(fmt::format("Error type {} is not accounted for! Take the developer's Kolkata privileges away...\n",
                                       errorTypes().keyword(errorType))));
    return 0.0;
}

// Return error of specified type between supplied double vectors
double error(ErrorType errorType, const std::vector<double> &vecA, const std::vector<double> &vecB, bool quiet)
{
    // Size check
    assert(vecA.size() == vecB.size());

    // Create temporary Data1D for simplicity
    Data1D A, B;
    auto x = 0.0;
    for (auto &&[y1, y2] : zip(vecA, vecB))
    {
        A.addPoint(x, y1);
        B.addPoint(x, y2);
        x += 1.0;
    }

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

// Return error of specified type between supplied data
double error(ErrorType errorType, const Data1D &A, const Data1D &B, bool quiet, Range range)
{

    // if range is unset
    if (range.maximum() == range.minimum())
    {
        range.set(B.xAxis().front(), B.xAxis.back())
    }

    if (errorType == RMSEError)
        return rmse(ATrimmed, BTrimmed, quiet, range);
    else if (errorType == MAAPEError)
        return maape(ATrimmed, BTrimmed, quiet, range);
    else if (errorType == MAPEError)
        return mape(ATrimmed, BTrimmed, quiet, range);
    else if (errorType == PercentError)
        return percent(ATrimmed, BTrimmed, quiet, range);
    else if (errorType == RFactorError)
        return rFactor(ATrimmed, BTrimmed, quiet, range);
    else if (errorType == EuclideanError)
        return euclidean(ATrimmed, BTrimmed, quiet, range);

    throw(
        std::runtime_error(fmt::format("Error type {} is not accounted for! Take the developer's Kolkata privileges away...\n",
                                       errorTypes().keyword(errorType))));
    return 0.0;
}

/*
 * Data1D
 */

// Return RMSE between supplied data
double rmse(const Data1D &A, const Data1D &B, bool quiet, Range range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    if (range.maximum() == range.minimum())
    {
        range.set(B.xAxis().front(), B.xAxis.back())
    }

    // Generate RMSE at x values of A
    auto rmse = 0.0, firstX = 0.0, lastX = 0.0;
    double delta;
    auto nPointsConsidered = 0;

    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < range.minimum())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > range.maximum())
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
double mape(const Data1D &A, const Data1D &B, bool quiet, Range range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    auto sum = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < range.minimum())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > range.maximum())
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

    auto mape = 100.0 * sum / nPointsConsidered;
    if (!quiet)
        Messenger::print("MAPE between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n", mape, firstX, lastX,
                         nPointsConsidered);

    return mape;
}

// Return MAAPE between supplied data
double maape(const Data1D &A, const Data1D &B, bool quiet, Range range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    auto sum = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < range.minimum())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > range.maximum())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Accumulate sum
        sum += atan(fabs((y - interpolatedB.y(x)) / y));

        lastX = x;
        ++nPointsConsidered;
    }

    auto maape = 100.0 * sum / nPointsConsidered;
    if (!quiet)
        Messenger::print("MAAPE between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n", maape, firstX, lastX,
                         nPointsConsidered);

    return maape;
}

// Return percentage error between supplied data
double percent(const Data1D &A, const Data1D &B, bool quiet, Range range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    if (range.maximum() == range.minimum())
    {
        range.set(B.xAxis().front(), B.xAxis.back())
    }

    // Calculate summed absolute error and absolute y value deviations from average
    auto sume = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < range.minimum())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > range.maximum())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Get y reference value
        sume += fabs(y - interpolatedB.y(x));
        sumy += fabs(y);

        lastX = x;
        ++nPointsConsidered;
    }

    // Calculate percentage error, avoiding divide-by-zero if the sum of y values is zero
    auto zeroSum = sumy == 0;
    auto percentError = (zeroSum ? sume : 100.0 * sume / sumy);
    if (!quiet)
    {
        if (zeroSum)
            Messenger::print("Absolute squared error between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n",
                             percentError, firstX, lastX, nPointsConsidered);
        else
            Messenger::print("Percentage error between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n",
                             percentError, firstX, lastX, nPointsConsidered);
    }

    return percentError;
}

// Return R-Factor (average squared error per point) between supplied data
double rFactor(const Data1D &A, const Data1D &B, bool quiet, Range range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    if (range.maximum() == range.minimum())
    {
        range.set(B.xAxis().front(), B.xAxis.back())
    }

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    // Accumulate sum-of-squares error at x values of A
    auto rfac = 0.0, delta = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(aX, aY))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < range.minimum())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > range.maximum())
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
double euclidean(const Data1D &A, const Data1D &B, bool quiet, Range range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    if (range.maximum() == range.minimum())
    {
        range.set(B.xAxis().front(), B.xAxis.back())
    }

    auto y2 = 0.0, sos = 0.0, delta = 0.0;
    auto firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < range.minimum())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > range.maximum())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        delta = y - interpolatedB.y(x);
        sos += delta * delta;
        y2 += y * y;

        lastX = x;
        ++nPointsConsidered;
    }

    // Calculate final error and summarise result
    auto euc = sos / sqrt(y2);
    if (!quiet)
        Messenger::print("Euclidean distance between datasets is {:15.9e} over {:15.9e} < x < {:15.9e} ({} points).\n", euc,
                         firstX, lastX, nPointsConsidered);

    return euc;
}

} // namespace Error
