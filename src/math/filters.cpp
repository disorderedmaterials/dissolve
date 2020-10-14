// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "math/filters.h"
#include "math/data1d.h"
#include "math/integrator.h"
#include "templates/array.h"

// Perform point-wise convolution of data with the supplied BroadeningFunction
void Filters::convolve(Data1D &data, const BroadeningFunction &function, bool variableOmega, bool normalise)
{
    // Check for no broadening function specified
    if (function.function() == BroadeningFunction::NoFunction)
        return;

    // Grab x and y arrays
    const auto &x = data.constXAxis();
    const auto xDelta = x.constAt(1) - x.constAt(0);
    Array<double> &y = data.values();

    Array<double> newY(data.nValues());

    // Outer loop over existing data points - if variableOmega == true then we use the x value as the omega broadening
    // parameter
    double xCentre, xBroad, norm;
    if (variableOmega)
        for (int n = 0; n < x.nItems(); ++n)
        {
            // Grab x value as our current xCentre
            xCentre = x.constAt(n);

            // Get normalisation for this convolution
            norm = (normalise ? function.discreteKernelNormalisation(xDelta, xCentre) : 1.0);

            // Inner loop over whole array
            for (int m = 0; m < x.nItems(); ++m)
            {
                xBroad = x.constAt(m) - xCentre;
                newY[m] += y.constAt(n) * function.y(xBroad, xCentre) * norm;
            }
        }
    else
    {
        // Get normalisation for this convolution
        norm = (normalise ? function.discreteKernelNormalisation(xDelta) : 1.0);

        for (int n = 0; n < x.nItems(); ++n)
        {
            // Grab x value as our current xCentre
            xCentre = x.constAt(n);

            // Inner loop over whole array
            for (int m = 0; m < x.nItems(); ++m)
            {
                xBroad = x.constAt(m) - xCentre;
                newY[m] += y.constAt(n) * function.y(xBroad) * norm;
            }
        }
    }

    y = newY;
}

// Perform convolution of the supplied delta function into the supplied data
void Filters::convolve(double xCentre, double value, const BroadeningFunction &function, Data1D &dest)
{
    // Check for no broadening function specified
    if (function.function() == BroadeningFunction::NoFunction)
        return;

    // Grab x and y arrays
    const auto &x = dest.constXAxis();
    Array<double> &y = dest.values();

    // Loop over existing datapoints
    double xBroad;
    for (int n = 0; n < x.nItems(); ++n)
    {
        xBroad = x.constAt(n) - xCentre;
        y[n] += value * function.y(xBroad);
    }
}

// Apply Kolmogorov–Zurbenko filter
void Filters::kolmogorovZurbenko(Data1D &data, int k, int m, bool normalised)
{
    for (int iteration = 0; iteration < k; ++iteration)
        normalised ? normalisedMovingAverage(data, m) : movingAverage(data, m);
}

// Apply median filter to data
void Filters::median(Data1D &data, int length)
{
    // Grab y array
    Array<double> &y = data.values();

    double window[length], avg, result;
    int m, i = length / 2, n = length / 2, miny, maxy;
    Array<double> newY(data.nValues());

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
    y = newY;
}

// Perform moving average smoothing
void Filters::movingAverage(Data1D &data, int avgSize)
{
    // Grab y array
    Array<double> &y = data.values();

    // Make sure avgSize is odd
    if (avgSize % 2 == 0)
        --avgSize;

    Array<double> newY(data.nValues());
    newY = 0.0;
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
void Filters::normalisedMovingAverage(Data1D &data, int avgSize)
{
    // Calculate the original integral
    double originalIntegral = Integrator::absTrapezoid(data);

    // Perform the smoothing
    movingAverage(data, avgSize);

    // Calculate the new integral
    double newIntegral = Integrator::absTrapezoid(data);

    data.values() *= (originalIntegral / newIntegral);
}

// Subtract average level from data, forming average from supplied x value
double Filters::subtractAverage(Data1D &data, double xStart)
{
    // Grab x and y arrays
    const auto &x = data.constXAxis();
    Array<double> &y = data.values();

    double sum = 0.0;
    auto nPoints = 0;
    for (int n = 0; n < x.nItems(); ++n)
    {
        if (x.constAt(n) >= xStart)
        {
            sum += y[n];
            ++nPoints;
        }
    }

    y -= sum / nPoints;

    return sum / nPoints;
}

// Trim supplied data to specified range
void Filters::trim(Data1D &data, double xMin, double xMax, bool interpolateEnds, double interpolationThreshold)
{
    Array<double> newX, newY;
    const auto &x = data.constXAxis();
    for (int n = 0; n < x.nItems(); ++n)
    {
        if (x.constAt(n) < xMin)
            continue;
        if (x.constAt(n) > xMax)
        {
            // X axis value now exceeds the xMax - interpolate the end?
            if (interpolateEnds)
            {
                // Is there a usable data point with lower x than the present one that we can use for our
                // interpolation?
                if (n > 0)
                {
                    double intervalFraction = (xMax - x.constAt(n - 1)) / (x.constAt(n) - x.constAt(n - 1));
                    if ((1.0 - intervalFraction) > interpolationThreshold)
                    {
                        newX.add(xMax);
                        newY.add(data.value(n - 1) + intervalFraction * (data.value(n) - data.value(n - 1)));
                    }
                }
            }
            break;
        }

        // If this is the first point we add, should we interpolate its value?
        if ((newX.nItems() == 0) && interpolateEnds)
        {
            // Is there a usable data point with lower x than the present one that we can use for our interpolation?
            if (n > 0)
            {
                double intervalFraction = (xMin - x.constAt(n - 1)) / (x.constAt(n) - x.constAt(n - 1));
                if ((1.0 - intervalFraction) > interpolationThreshold)
                {
                    newX.add(xMin);
                    newY.add(data.value(n - 1) + intervalFraction * (data.value(n) - data.value(n - 1)));
                }
            }
        }
        newX.add(x.constAt(n));
        newY.add(data.value(n));
    }

    // Set new arrays
    data.xAxis() = newX;
    data.values() = newY;
}

// Convert bin boundaries to centre-bin values
void Filters::convertBinBoundaries(Data1D &data)
{
    // Assume that input x values are histogram bin left-boundaries, so x(n) = 0.5[x(n)+x(n_1)]
    Array<double> &x = data.xAxis();
    double a = x[0], b;
    for (int n = 0; n < data.nValues() - 1; ++n)
    {
        b = x[n + 1];
        x[n] = 0.5 * (a + b);
        a = b;
    }

    // Remove last point
    data.removeLastPoint();
}
