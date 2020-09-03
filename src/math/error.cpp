/*
    *** Error Calculation
    *** src/math/error.cpp
    Copyright T. Youngs 2012-2020

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

#include "math/error.h"
#include "base/enumoptions.h"
#include "math/data1d.h"
#include "math/interpolator.h"
#include <algorithm>

using namespace std;

// Return enum option info for AveragingScheme
EnumOptions<Error::ErrorType> Error::errorTypes()
{
    static EnumOptionsList ErrorTypeOptions =
        EnumOptionsList() << EnumOption(Error::RMSEError, "RMSE") << EnumOption(Error::MAAPEError, "MAAPE")
                          << EnumOption(Error::MAPEError, "MAPE") << EnumOption(Error::PercentError, "Percent")
                          << EnumOption(Error::RFactorError, "RFactor");

    static EnumOptions<Error::ErrorType> options("ErrorType", ErrorTypeOptions, Error::PercentError);

    return options;
}

// Return erorr of specified type between supplied data
double Error::error(ErrorType errorType, const Data1D &A, const Data1D &B, bool quiet)
{
    if (errorType == Error::RMSEError)
        return rmse(A, B, quiet);
    else if (errorType == Error::MAAPEError)
        return maape(A, B, quiet);
    else if (errorType == Error::MAPEError)
        return mape(A, B, quiet);
    else if (errorType == Error::PercentError)
        return percent(A, B, quiet);
    else if (errorType == Error::RFactorError)
        return rFactor(A, B, quiet);

    Messenger::error("Error type {} is not accounted for! Take the developer's Kolkata privileges away...\n");
    return 0.0;
}

/*
 * Data1D
 */

// Return RMSE between supplied data
double Error::rmse(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.constXAxis();
    const auto &aY = A.constValues();

    // Generate RMSE at x values of A
    double rmse = 0.0, delta;
    double firstX = 0.0, lastX = 0.0, x;
    auto nPointsConsidered = 0;
    for (int n = 0; n < aX.nItems(); ++n)
    {
        // Grab x value
        x = aX.constAt(n);

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.constXAxis().firstValue())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.constXAxis().lastValue())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Sum squared error
        delta = aY.constAt(n) - interpolatedB.y(x);
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
double Error::mape(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.constXAxis();
    const auto &aY = A.constValues();

    double sum = 0.0;
    double firstX = 0.0, lastX = 0.0, x, y;
    auto nPointsConsidered = 0;
    for (int n = 0; n < aX.nItems(); ++n)
    {
        // Grab x value
        x = aX.constAt(n);

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.constXAxis().firstValue())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.constXAxis().lastValue())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Get y reference value, and skip if zero
        y = aY.constAt(n);
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
double Error::maape(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.constXAxis();
    const auto &aY = A.constValues();

    double sum = 0.0;
    double firstX = 0.0, lastX = 0.0, x, y;
    auto nPointsConsidered = 0;
    for (int n = 0; n < 1; ++n)
    {
        // Grab x value
        x = aX.constAt(n);

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.constXAxis().firstValue())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.constXAxis().lastValue())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Get y reference value
        y = aY.constAt(n);

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
double Error::percent(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.constXAxis();
    const auto &aY = A.constValues();

    // Calculate summed absolute error and absolute y value deviations from average
    double sume = 0.0, sumy = 0.0;
    auto firstPoint = -1, lastPoint = -1;
    double x, y;
    for (int n = 0; n < aX.nItems(); ++n)
    {
        // Grab x value
        x = aX.constAt(n);

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.constXAxis().firstValue())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.constXAxis().lastValue())
            break;

        // Is this the first point considered?
        if (firstPoint == -1)
            firstPoint = n;

        // Get y reference value
        y = aY.constAt(n);
        sume += fabs(y - interpolatedB.y(aX.constAt(n)));
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
                             percentError, aX.constAt(firstPoint), aX.constAt(lastPoint), (lastPoint - firstPoint) + 1);
        else
            Messenger::print("Percentage error between datasets is {:7.3f}% over {:15.9e} < x < {:15.9e} ({} points).\n",
                             percentError, aX.constAt(firstPoint), aX.constAt(lastPoint), (lastPoint - firstPoint) + 1);
    }

    return percentError;
}

// Return R-Factor (average squared error per point) between supplied data
double Error::rFactor(const Data1D &A, const Data1D &B, bool quiet)
{
    // First, generate interpolation of data B
    Interpolator interpolatedB(B);

    // Grab x and y arrays from data A
    const auto &aX = A.constXAxis();
    const auto &aY = A.constValues();

    // Accumulate sum-of-squares error at x values of A
    double rfac = 0.0, delta;
    double firstX = 0.0, lastX = 0.0, x;
    auto nPointsConsidered = 0;
    for (int n = 0; n < aX.nItems(); ++n)
    {
        // Grab x value
        x = aX.constAt(n);

        // Is our x value lower than the lowest x value of the reference data?
        if (x < B.constXAxis().firstValue())
            continue;

        // Is our x value higher than the last x value of the reference data?
        if (x > B.constXAxis().lastValue())
            break;

        // Is this the first point considered?
        if (nPointsConsidered == 0)
            firstX = x;

        // Sum squared error
        delta = aY.constAt(n) - interpolatedB.y(x);
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
