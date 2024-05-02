// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/integrator.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/algorithms.h"
#include <numeric>

/*
 * Static Functions
 */

// Compute integral of supplied data via trapezoid rule
double Integrator::trapezoid(const Data1D &data)
{
    // Check for insufficient data
    if (data.nValues() < 2)
        return 0.0;

    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &y = data.values();

    auto total = 0.0, y0 = y.front(), x0 = x.front();
    for (auto &&[x1, y1] : zip(x, y))
    {
        total += (x1 - x0) * (y0 + y1) * 0.5;
        x0 = x1;
        y0 = y1;
    }
    return total;
}

// Compute integral of supplied data via trapezoid rule between the specified limits
double Integrator::trapezoid(const Data1D &data, double xMin, double xMax)
{
    // Check for insufficient data
    if (data.nValues() < 2)
        return 0.0;

    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &y = data.values();

    double total = 0.0, y0 = 0.0, x0 = 0.0;
    auto nPoints = 0;
    for (auto &&[x1, y1] : zip(x, y))
    {
        if (x1 < xMin)
            continue;
        if (x1 > xMax)
            break;

        // If this is the first point, don't try to increase integral
        if (nPoints > 0)
            total += (x1 - x0) * (y0 + y1) * 0.5;

        // Store old values
        x0 = x1;
        y0 = y1;

        ++nPoints;
    }

    return total;
}

// Compute integral of supplied data via trapezoid rule within the specified range
double Integrator::trapezoid(const Data1D &data, const Range range)
{
    return trapezoid(data, range.minimum(), range.maximum());
}

// Compute absolute integral of supplied data via trapezoid rule
double Integrator::absTrapezoid(const Data1D &data)
{
    // Check for insufficient data
    if (data.nValues() < 2)
        return 0.0;

    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &y = data.values();

    auto total = 0.0, y0 = y.front(), x0 = x.front();
    for (auto &&[x1, y1] : zip(x, y))
    {
        total += fabs((x1 - x0) * (y0 + y1) * 0.5);
        x0 = x1;
        y0 = y1;
    }
    return total;
}

// Return sum of all values in supplied data
double Integrator::sum(const Data1D &data)
{
    // Grab data array
    const auto &values = data.values();

    auto total = 0.0;

    for (double value : values)
        total += value;

    return total;
}

// Return sum of supplied data between the specified limits
double Integrator::sum(const Data1D &data, double xMin, double xMax)
{
    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &values = data.values();

    auto total = 0.0;

    for (auto &&[xn, value] : zip(x, values))
    {
        if (xn < xMin)
            continue;
        if (xn > xMax)
            break;

        total += value;
    }

    return total;
}

// Return sum of supplied data within the specified range
double Integrator::sum(const Data1D &data, const Range range) { return sum(data, range.minimum(), range.maximum()); }

// Return sum of all absolute values of supplied data
double Integrator::absSum(const Data1D &data)
{
    // Grab data array
    const auto &values = data.values();

    auto total = 0.0;

    for (double value : values)
        total += fabs(value);

    return total;
}

// Return sum of all absolute values between the specified limits
double Integrator::absSum(const Data1D &data, double xMin, double xMax)
{
    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &values = data.values();

    auto total = 0.0;

    for (auto &&[xn, value] : zip(x, values))
    {
        if (xn < xMin)
            continue;
        if (xn > xMax)
            break;

        total += fabs(value);
    }

    return total;
}

// Return sum of all absolute values within the specified range
double Integrator::absSum(const Data1D &data, const Range range) { return absSum(data, range.minimum(), range.maximum()); }

// Return sum of squares of all values in supplied data
double Integrator::sumOfSquares(const Data1D &data)
{
    // Grab data array
    const auto &values = data.values();

    auto total = 0.0;

    for (double value : values)
        total += value * value;

    return total;
}

// Return sum of squares of values between the specified limits
double Integrator::sumOfSquares(const Data1D &data, double xMin, double xMax)
{
    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &values = data.values();

    auto total = 0.0;

    for (auto &&[xn, value] : zip(x, values))
    {
        if (xn < xMin)
            continue;
        if (xn > xMax)
            break;

        total += value * value;
    }

    return total;
}

// Return sum of squares of values within the specified range
double Integrator::sumOfSquares(const Data1D &data, const Range range)
{
    return sumOfSquares(data, range.minimum(), range.maximum());
}

// Return sum of all values in supplied data
double Integrator::sum(const Data2D &data) { return std::accumulate(data.values().begin(), data.values().end(), 0.0); }

// Return sum of all absolute values in supplied data
double Integrator::absSum(const Data2D &data)
{
    return std::accumulate(data.values().begin(), data.values().end(), 0.0, [](auto a, auto b) { return fabs(a) + fabs(b); });
}

// Return sum of all values in supplied data
double Integrator::sum(const Data3D &data)
{
    // Grab data array
    const Array3D<double> &values = data.values();

    return std::accumulate(values.linearArray().begin(), values.linearArray().end(), 0.0);
}

// Return sum of all absolute values in supplied data
double Integrator::absSum(const Data3D &data)
{
    // Grab data array
    const Array3D<double> &values = data.values();

    return std::accumulate(values.linearArray().begin(), values.linearArray().end(), 0.0,
                           [](auto acc, auto n) { return acc + fabs(n); });
}
