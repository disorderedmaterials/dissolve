// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/extrema.h"
#include "templates/array.h"
#include "templates/array2d.h"

namespace Extrema
{
// Return minimum from 1D array provided
double min(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        double min = A.at(0);
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (A.at(i) < min)
                min = A.at(i);
        }
        return min;
    }
    else
        return 0;
}

// Return maximum from 1D array provided
double max(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        double max = A.at(0);
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (A.at(i) > max)
                max = A.at(i);
        }
        return max;
    }
    else
        return 0;
}

// Return minimum from 2D array provided
double min(const Array2D<double> &A)
{
    if (A.empty())
        return 0;
    return *std::min_element(A.begin(), A.end());
}

// Return maximum from 2D array provided
double max(const Array2D<double> &A)
{
    if (A.empty())
        return 0;
    return *std::max_element(A.begin(), A.end());
}

// Return absolute minimum from array provided
double absMin(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        auto absMin = fabs(A.at(0));
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (fabs(A.at(i)) < absMin)
                absMin = fabs(A.at(i));
        }
        return absMin;
    }
    else
        return 0;
}

// Return absolute maximum from array provided
double absMax(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        auto absMax = fabs(A.at(0));
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (fabs(A.at(i)) > absMax)
                absMax = fabs(A.at(i));
        }
        return absMax;
    }
    else
        return 0;
}

// Return absolute minimum from 2D array provided
double absMin(const Array2D<double> &A)
{
    if (A.empty())
        return 0;
    return *std::min_element(A.begin(), A.end(), [](auto a, auto b) { return fabs(a) < fabs(b); });
}

// Return absolute maximum from 2D array provided
double absMax(const Array2D<double> &A)
{
    if (A.empty())
        return 0;
    return *std::max_element(A.begin(), A.end(), [](auto a, auto b) { return fabs(a) < fabs(b); });
}
} // namespace Extrema