// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "math/extrema.h"
#include "templates/array.h"
#include "templates/array2d.h"

// Return minimum from 1D array provided
double Extrema::min(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        double min = A.constAt(0);
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (A.constAt(i) < min)
                min = A.constAt(i);
        }
        return min;
    }
    else
        return 0;
}

// Return maximum from 1D array provided
double Extrema::max(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        double max = A.constAt(0);
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (A.constAt(i) > max)
                max = A.constAt(i);
        }
        return max;
    }
    else
        return 0;
}

// Return minimum from 2D array provided
double Extrema::min(const Array2D<double> &A)
{
    if (!A.empty())
        return *std::min_element(A.begin(), A.end());
    else
        return 0;
}

// Return maximum from 2D array provided
double Extrema::max(const Array2D<double> &A)
{
    if (!A.empty())
        return *std::max_element(A.begin(), A.end());
    else
        return 0;
}

// Return absolute minimum from array provided
double Extrema::absMin(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        double absMin = fabs(A.constAt(0));
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (fabs(A.constAt(i)) < absMin)
                absMin = fabs(A.constAt(i));
        }
        return absMin;
    }
    else
        return 0;
}

// Return absolute maximum from array provided
double Extrema::absMax(const Array<double> &A)
{
    if (A.nItems() > 0)
    {
        double absMax = fabs(A.constAt(0));
        for (auto i = 0; i < A.nItems(); ++i)
        {
            if (fabs(A.constAt(i)) > absMax)
                absMax = fabs(A.constAt(i));
        }
        return absMax;
    }
    else
        return 0;
}

// Return absolute minimum from 2D array provided
double Extrema::absMin(const Array2D<double> &A)
{
    if (!A.empty())
        return *std::min_element(A.begin(), A.end(), [](auto a, auto b) { return fabs(a) < fabs(b); });
    else
        return 0;
}

// Return absolute maximum from 2D array provided
double Extrema::absMax(const Array2D<double> &A)
{
    if (!A.empty())
        return *std::max_element(A.begin(), A.end(), [](auto a, auto b) { return fabs(a) < fabs(b); });
    else
        return 0;
}
