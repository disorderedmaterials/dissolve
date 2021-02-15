// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/filters.h"
#include "math/data1d.h"
#include "math/integrator.h"
#include "templates/algorithms.h"
#include "templates/array.h"

namespace Filters
{
// Perform point-wise convolution of data with the supplied BroadeningFunction
void convolve(Data1D &data, const BroadeningFunction &function, bool variableOmega, bool normalise)
{
    // Check for no broadening function specified
    if (function.function() == BroadeningFunction::NoFunction)
        return;

    // Grab x and y arrays
    const auto &x = data.xAxis();
    const auto xDelta = x[1] - x[0];
    auto &y = data.values();

    std::vector<double> newY(data.nValues());

    // Outer loop over existing data points - if variableOmega == true then we use the x value as the omega broadening
    // parameter
    double norm, xBroad;
    if (variableOmega)
        for (auto &&[xCentre, yn] : zip(x, y))
        {
            // Get normalisation for this convolution
            norm = (normalise ? function.discreteKernelNormalisation(xDelta, xCentre) : 1.0);

            // Inner loop over whole array
            for (auto &&[xm, newYm] : zip(x, newY))
            {
                xBroad = xm - xCentre;
                newYm += yn * function.y(xBroad, xCentre) * norm;
            }
        }
    else
    {
        // Get normalisation for this convolution
        norm = (normalise ? function.discreteKernelNormalisation(xDelta) : 1.0);

        for (auto &&[xCentre, yn] : zip(x, y))
        {
            // Inner loop over whole array
            std::transform(x.begin(), x.end(), newY.begin(), newY.begin(),
                           [yn = yn, &function, norm, xCentre = xCentre](auto X, auto NewY) {
                               return NewY + yn * function.y(X - xCentre) * norm;
                           });
        }
    }

    y = newY;
}

// Perform convolution of the supplied delta function into the supplied data
void convolve(double xCentre, double value, const BroadeningFunction &function, Data1D &dest)
{
    // Check for no broadening function specified
    if (function.function() == BroadeningFunction::NoFunction)
        return;

    // Grab x and y arrays
    const auto &x = dest.xAxis();
    auto &y = dest.values();

    // Loop over existing datapoints
    std::transform(x.begin(), x.end(), y.begin(), y.begin(),
                   [&](auto x, auto y) { return y + value * function.y(x - xCentre); });
}

// Apply Kolmogorov–Zurbenko filter
void kolmogorovZurbenko(Data1D &data, int k, int m, bool normalised)
{
    for (auto iteration = 0; iteration < k; ++iteration)
        normalised ? normalisedMovingAverage(data, m) : movingAverage(data, m);
}

// Apply median filter to data
void median(Data1D &data, int length)
{
    // Grab y array
    auto &y = data.values();

    std::vector<double> window(length);
    double avg, result;
    int m, i = length / 2, n = length / 2, miny, maxy;
    std::vector<double> newY(data.nValues());

    // Set boundary values
    for (m = 0; m < n; ++m)
    {
        newY[m] = y[m];
        newY[data.nValues() - 1 - m] = y[data.nValues() - 1 - m];
    }

    // Now loop over remaining points
    while (n < (data.nValues() - i))
    {
        // Grab data values, and determine min/max values
        miny = 0;
        maxy = 0;
        for (m = -i; m <= i; ++m)
        {
            window[m + i] = y[n + m];
            if (window[m + i] < window[miny])
                miny = m + i;
            if (window[m + i] > window[maxy])
                maxy = m + i;
        }

        // Determine median value without sorting. First, calculate average value...
        avg = 0.0;
        for (m = 0; m < length; ++m)
            if ((m != miny) && (m != maxy))
                avg += window[m];
        avg /= length - 2;

        // Now find value closest to the average
        result = window[0];
        for (m = 0; m < length; ++m)
            if ((m != miny) && (m != maxy) && (fabs(window[m] - avg) < fabs(result - avg)))
                result = window[m];

        // Store median value
        newY[n] = result;

        ++n;
    }

    // Store new values
    std::copy(newY.begin(), newY.end(), y.begin());
}

// Perform moving average smoothing
void movingAverage(Data1D &data, int avgSize)
{
    // Grab y array
    auto &y = data.values();

    // Make sure avgSize is odd
    if (avgSize % 2 == 0)
        --avgSize;

    std::vector<double> newY(data.nValues());
    std::fill(newY.begin(), newY.end(), 0.0);
    int n, m, i = avgSize / 2;

    // Left-most region of data
    for (n = 0; n < i; ++n)
    {
        for (m = 0; m <= n + i; ++m)
            newY[n] += y[m];
        newY[n] /= (i + 1 + n);
    }

    // Central region (full average width available)
    for (n = i; n < data.nValues() - i; ++n)
    {
        for (m = n - i; m <= n + i; ++m)
            newY[n] += y[m];
        newY[n] /= avgSize;
    }

    // Right-most region of data
    for (n = data.nValues() - i; n < data.nValues(); ++n)
    {
        for (m = n - i; m < data.nValues(); ++m)
            newY[n] += y[m];
        newY[n] /= (data.nValues() - n + i + 1);
    }

    y = newY;
}

// Perform moving average smoothing, normalising area after smooth
void normalisedMovingAverage(Data1D &data, int avgSize)
{
    // Calculate the original integral
    auto originalIntegral = Integrator::absTrapezoid(data);

    // Perform the smoothing
    movingAverage(data, avgSize);

    // Calculate the new integral
    auto newIntegral = Integrator::absTrapezoid(data);

    data *= originalIntegral / newIntegral;
}

// Subtract average level from data, forming average from supplied x value
double subtractAverage(Data1D &data, double xStart)
{
    // Grab x and y arrays
    const auto &x = data.xAxis();
    auto &y = data.values();

    auto sum = 0.0;
    auto nPoints = 0;
    for (auto n = 0; n < x.size(); ++n)
    {
        if (x[n] >= xStart)
        {
            sum += y[n];
            ++nPoints;
        }
    }

    std::transform(y.begin(), y.end(), y.begin(), [sum, nPoints](auto value) { return value - sum / nPoints; });

    return sum / nPoints;
}

// Trim supplied data to specified range
void trim(Data1D &data, double xMin, double xMax, bool interpolateEnds, double interpolationThreshold)
{
    std::vector<double> newX, newY;
    const auto &x = data.xAxis();
    for (auto n = 0; n < x.size(); ++n)
    {
        if (x[n] < xMin)
            continue;
        if (x[n] > xMax)
        {
            // X axis value now exceeds the xMax - interpolate the end?
            if (interpolateEnds)
            {
                // Is there a usable data point with lower x than the present one that we can use for our
                // interpolation?
                if (n > 0)
                {
                    double intervalFraction = (xMax - x[n - 1]) / (x[n] - x[n - 1]);
                    if ((1.0 - intervalFraction) > interpolationThreshold)
                    {
                        newX.push_back(xMax);
                        newY.push_back(data.value(n - 1) + intervalFraction * (data.value(n) - data.value(n - 1)));
                    }
                }
            }
            break;
        }

        // If this is the first point we add, should we interpolate its value?
        if (newX.empty() && interpolateEnds)
        {
            // Is there a usable data point with lower x than the present one that we can use for our interpolation?
            if (n > 0)
            {
                double intervalFraction = (xMin - x[n - 1]) / (x[n] - x[n - 1]);
                if ((1.0 - intervalFraction) > interpolationThreshold)
                {
                    newX.push_back(xMin);
                    newY.push_back(data.value(n - 1) + intervalFraction * (data.value(n) - data.value(n - 1)));
                }
            }
        }
        newX.push_back(x[n]);
        newY.push_back(data.value(n));
    }

    // Set new arrays
    data.xAxis() = newX;
    data.values() = newY;
}

// Trim supplied data to be the same range as the reference data
void trim(Data1D &data, const Data1D &ref, bool interpolateEnds, double interpolationThreshold)
{
    trim(data, ref.xAxis().front(), ref.xAxis().back(), interpolateEnds, interpolationThreshold);
}

// Convert bin boundaries to centre-bin values
void convertBinBoundaries(Data1D &data)
{
    // Assume that input x values are histogram bin left-boundaries, so x(n) = 0.5[x(n)+x(n_1)]
    auto &x = data.xAxis();
    auto a = x[0], b = 0.0;
    for (auto n = 0; n < data.nValues() - 1; ++n)
    {
        b = x[n + 1];
        x[n] = 0.5 * (a + b);
        a = b;
    }

    // Remove last point
    data.removeLastPoint();
}
} // namespace Filters
