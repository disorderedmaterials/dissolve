// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class T> class Array;
template <class T> class Array2D;
template <class T> class Array3D;

// Extrema
namespace Extrema
{
// Return minimum from array provided
double min(const Array<double> &A);
// Return maximum from array provided
double max(const Array<double> &A);
// Return minimum from 2D array provided
double min(const Array2D<double> &A);
// Return maximum from 2D array provided
double max(const Array2D<double> &A);
// Return absolute minimum from array provided
double absMin(const Array<double> &A);
// Return absolute maximum from array provided
double absMax(const Array<double> &A);
// Return absolute minimum from 2D array provided
double absMin(const Array2D<double> &A);
// Return absolute maximum from 2D array provided
double absMax(const Array2D<double> &A);
}; // namespace Extrema
