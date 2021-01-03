// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class A> class Array2D;

// Single Value Decomposition
class SVD
{
    public:
    // Perform single value decomposition of the supplied matrix A, putting left-orthogonal (U), diagonal single-value (S),
    // and right-orthogonal (V) matrices into the supplied Arrays
    static bool decompose(const Array2D<double> &A, Array2D<double> &U, Array2D<double> &S, Array2D<double> &Vt);
    // Test SVD
    static void test();
    // Compute in-place pseudoinverse of supplied matrix
    static bool pseudoinverse(Array2D<double> &A);
};
