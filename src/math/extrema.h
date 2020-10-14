// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class T> class Array;
template <class T> class Array2D;
template <class T> class Array3D;

// Extrema
class Extrema
{
    public:
    // Return minimum from array provided
    static double min(const Array<double> &A);
    // Return maximum from array provided
    static double max(const Array<double> &A);
    // Return minimum from 2D array provided
    static double min(const Array2D<double> &A);
    // Return maximum from 2D array provided
    static double max(const Array2D<double> &A);
    // Return absolute minimum from array provided
    static double absMin(const Array<double> &A);
    // Return absolute maximum from array provided
    static double absMax(const Array<double> &A);
    // Return absolute minimum from 2D array provided
    static double absMin(const Array2D<double> &A);
    // Return absolute maximum from 2D array provided
    static double absMax(const Array2D<double> &A);
};
