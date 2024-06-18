// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/derivative.h"
#include "math/data1D.h"

namespace Derivative
{
// Return derivative of supplied Data1D
Data1D derivative(const Data1D &source)
{
    Data1D dYdX{source};
    std::fill(dYdX.values().begin(), dYdX.values().end(), 0.0);

    const auto nValues = source.nValues();
    if (nValues < 3)
        return dYdX;

    // Get spacing between points - we're assuming a constant spacing
    const auto delta = source.xAxis(1) - source.xAxis(0);

    for (auto n = 1; n < nValues - 1; ++n)
    {
        /* Calculate numerical derivative with five-point stencil if possible. Otherwise use three-point stencil.
         * Assumes data are regularly-spaced.
         *
         *            -f(x+2) + 8 f(x+1) - 8 f(x-1) + f(x-2)
         *    f'(x) = --------------------------------------
         *                           12 h
         */
        if ((n == 1) || (n == (nValues - 2)))
        {
            // Three-point
            dYdX.value(n) = -(source.value(n - 1) - source.value(n + 1)) / (2 * delta);
        }
        else
        {
            // Five-point stencil
            dYdX.value(n) =
                (-source.value(n + 2) + 8 * source.value(n + 1) - 8 * source.value(n - 1) + source.value(n - 2)) / (12 * delta);
        }
    }

    // Handle extreme points with a simple extrapolation
    dYdX.value(0) = dYdX.value(1) - (dYdX.value(1) - dYdX.value(2));
    dYdX.value(nValues - 1) = dYdX.value(nValues - 2) + (dYdX.value(nValues - 2) - dYdX.value(nValues - 3));

    return dYdX;
}
}; // namespace Derivative
