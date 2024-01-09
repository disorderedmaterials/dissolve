// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/regression.h"
#include "math/data1D.h"

namespace Regression
{
// Return gradient of last n points
double linear(const Data1D &data, int nSamples)
{
    double yMean;
    return linear(data, nSamples, yMean);
}

// Return gradient of last n points, along with average y value
double linear(const Data1D &data, int nSamples, double &yBar)
{
    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &y = data.values();

    double Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
    double xBar = 0.0;
    yBar = 0.0;

    // Calculate mean values of x and y
    for (auto n = data.nValues() - nSamples; n < data.nValues(); ++n)
    {
        xBar += x[n];
        yBar += y[n];
    }
    xBar /= nSamples;
    yBar /= nSamples;

    // Determine Sx, Sy, and Sxy
    double dx, dy;
    for (auto n = data.nValues() - nSamples; n < data.nValues(); ++n)
    {
        dx = x[n] - xBar;
        dy = y[n] - yBar;
        Sxx += dx * dx;
        Syy += dy * dy;
        Sxy += dx * dy;
    }

    // Return the gradient
    return Sxy / Sxx;
}
} // namespace Regression
