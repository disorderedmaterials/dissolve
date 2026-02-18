// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/regression.h"
#include "math/data1D.h"

namespace Regression
{
// Return gradient of last n points, along with average y value
double linearGradient(const Data1D &data, int nSamples, OptionalReferenceWrapper<double> returnYBar)
{
    // Grab data arrays
    const auto &x = data.xAxis();
    const auto &y = data.values();

    auto Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
    auto xBar = 0.0;
    auto yBar = 0.0;

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

    // Set the supplied external var if present
    if (returnYBar)
        returnYBar->get() = yBar;

    // Return the gradient
    return Sxy / Sxx;
}

// Return linear gradient of last n points, along with average y value, assuming unit spacing between points
double linearGradient(const std::vector<double> &y, int nSamples, OptionalReferenceWrapper<double> returnYBar)
{
    auto Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
    auto xBar = (nSamples - 1) / 2.0, yBar = 0.0;

    // Calculate mean values of x and y
    for (auto n = y.size() - nSamples; n < y.size(); ++n)
        yBar += y[n];
    yBar /= nSamples;

    // Determine Sx, Sy, and Sxy
    double dx, dy;
    for (auto n = 0; n < nSamples; ++n)
    {
        dx = n - xBar;
        auto i = y.size() - nSamples + n;
        dy = y[i] - yBar;
        Sxx += dx * dx;
        Syy += dy * dy;
        Sxy += dx * dy;
    }

    // Set the supplied external var if present
    if (returnYBar)
        returnYBar->get() = yBar;

    // Return the gradient
    return Sxy / Sxx;
}

} // namespace Regression
