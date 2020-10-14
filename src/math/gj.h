// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class A> class Array2D;

// Gauss-Jordan Inversion
class GaussJordan
{
    public:
    // Perform Gauss-Jordan inversion of the supplied Array2D<double>
    static bool invert(Array2D<double> &A);
};
