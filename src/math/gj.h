// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class A> class Array2D;

// Gauss-Jordan Inversion
namespace GaussJordan
{
// Perform Gauss-Jordan inversion of the supplied Array2D<double>
bool invert(Array2D<double> &A);
}; // namespace GaussJordan
