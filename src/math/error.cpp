// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/error.h"
#include "base/enumOptions.h"
#include "math/data1D.h"
#include "math/interpolator.h"
#include "templates/algorithms.h"
#include <algorithm>

namespace Error
{

// Return enum option info for AveragingScheme
EnumOptions<ErrorType> errorTypes()
{
    return EnumOptions<ErrorType>("ErrorType", {{RMSEError, "RMSE"},
                                                {MAAPEError, "MAAPE"},
                                                {MAPEError, "MAPE"},
                                                {PercentError, "Percent"},
                                                {ASEError, "ASE"},
                                                {RFactorError, "RFactor"},
                                                {EuclideanError, "Euclidean"}});
}

std::string errorReportString(const ErrorReport &errorReport)
{
    return fmt::format("{} between datasets is {:e} over {:e} < x < {:e} ({} points).\n",
                       errorTypes().keyword(errorReport.errorType), errorReport.error, errorReport.firstX, errorReport.lastX,
                       errorReport.nPointsConsidered);
}

// Return error of specified type between supplied data
ErrorReport error(ErrorType errorType, const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    if (errorType == RMSEError)
        return rmse(A, B, range);
    else if (errorType == MAAPEError)
        return maape(A, B, range);
    else if (errorType == MAPEError)
        return mape(A, B, range);
    else if (errorType == PercentError)
        return percent(A, B, range);
    else if (errorType == ASEError)
        return ase(A, B, range);
    else if (errorType == RFactorError)
        return rFactor(A, B, range);
    else if (errorType == EuclideanError)
        return euclidean(A, B, range);

    throw(
        std::runtime_error(fmt::format("Error type {} is not accounted for! Take the developer's Kolkata privileges away...\n",
                                       errorTypes().keyword(errorType))));
}

// Return error of specified type between supplied double vectors
ErrorReport error(ErrorType errorType, const std::vector<double> &vecA, const std::vector<double> &vecB)
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
        return rmse(A, B);
    else if (errorType == MAAPEError)
        return maape(A, B);
    else if (errorType == MAPEError)
        return mape(A, B);
    else if (errorType == PercentError)
        return percent(A, B);
    else if (errorType == ASEError)
        return ase(A, B);
    else if (errorType == RFactorError)
        return rFactor(A, B);
    else if (errorType == EuclideanError)
        return euclidean(A, B);

    throw(
        std::runtime_error(fmt::format("Error type {} is not accounted for! Take the developer's Kolkata privileges away...\n",
                                       errorTypes().keyword(errorType))));
}

/*
 * Data1D
 */

// Return RMSE between supplied data
ErrorReport rmse(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    // Generate RMSE at x values of A
    auto rmse = 0.0, firstX = 0.0, lastX = 0.0;
    double delta;
    auto nPointsConsidered = 0;

    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
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

    return ErrorReport{RMSEError, rmse, firstX, lastX, nPointsConsidered};
}

// Return MAPE between supplied data
ErrorReport mape(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    auto sum = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
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

    return ErrorReport{MAPEError, mape, firstX, lastX, nPointsConsidered};
}

// Return MAAPE between supplied data
ErrorReport maape(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    auto sum = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
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

    return ErrorReport{MAAPEError, maape, firstX, lastX, nPointsConsidered};
}

// Return percentage error between supplied data
ErrorReport percent(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    // Calculate summed absolute error and absolute y value deviations from average
    auto sume = 0.0, sumy = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
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
    // (Returns absolute squared error instead)

    if (sumy == 0)
    {
        return ErrorReport{ASEError, sume, firstX, lastX, nPointsConsidered};
    }

    auto percentError = 100.0 * sume / sumy;
    return ErrorReport{PercentError, percentError, firstX, lastX, nPointsConsidered};
}

// Return absolute squared error between supplied data
ErrorReport ase(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    // Calculate summed absolute error and absolute y value deviations from average
    auto sume = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Get y reference value
        sume += fabs(y - interpolatedB.y(x));

        lastX = x;
        ++nPointsConsidered;
    }

    // Calculate percentage error, avoiding divide-by-zero if the sum of y values is zero
    // (Returns absolute squared error instead)

    return ErrorReport{ASEError, sume, firstX, lastX, nPointsConsidered};
}

// Return R-Factor (average squared error per point) between supplied data
ErrorReport rFactor(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    // Grab x and y arrays from data A
    const auto &aX = A.xAxis();
    const auto &aY = A.values();

    // Accumulate sum-of-squares error at x values of A
    auto rfac = 0.0, delta = 0.0, firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(aX, aY))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
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

    return ErrorReport{RFactorError, rfac, firstX, lastX, nPointsConsidered};
}

// Return Euclidean distance, normalised to mean of B, between supplied data
ErrorReport euclidean(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // if range is unset
    auto rangeMin = range ? range->get().minimum() : B.xAxis().front();
    auto rangeMax = range ? range->get().maximum() : B.xAxis().back();

    auto y2 = 0.0, sos = 0.0, delta = 0.0;
    auto firstX = 0.0, lastX = 0.0;
    auto nPointsConsidered = 0;
    for (auto &&[x, y] : zip(A.xAxis(), A.values()))
    {
        // Is our x value lower than the lowest x value of the reference data?
        if (x < rangeMin)
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > rangeMax)
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

    return ErrorReport{EuclideanError, euc, firstX, lastX, nPointsConsidered};
}

} // namespace Error
