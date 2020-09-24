// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "math/regression.h"
#include "math/data1d.h"
#include "templates/array.h"

// Return gradient of last n points
double Regression::linear(const Data1D &data, int nSamples)
{
    double yMean;
    return linear(data, nSamples, yMean);
}

// Return gradient of last n points, along with average y value
double Regression::linear(const Data1D &data, int nSamples, double &yBar)
{
    // Grab data arrays
    const auto &x = data.constXAxis();
    const auto &y = data.constValues();

    double Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
    double xBar = 0.0;
    yBar = 0.0;

    // Calculate mean values of x and y
    for (int n = data.nValues() - nSamples; n < data.nValues(); ++n)
    {
        xBar += x.constAt(n);
        yBar += y.constAt(n);
    }
    xBar /= nSamples;
    yBar /= nSamples;

    // Determine Sx, Sy, and Sxy
    double dx, dy;
    for (int n = data.nValues() - nSamples; n < data.nValues(); ++n)
    {
        dx = x.constAt(n) - xBar;
        dy = y.constAt(n) - yBar;
        Sxx += dx * dx;
        Syy += dy * dy;
        Sxy += dx * dy;
    }

    // Return the gradient
    return Sxy / Sxx;
}
