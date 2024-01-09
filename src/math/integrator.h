// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/range.h"

// Forward Declarations
class Data1D;
class Data2D;
class Data3D;

// Integrator
class Integrator
{
    /*
     * Static Functions
     */
    public:
    // Compute integral of supplied data via trapezoid rule
    static double trapezoid(const Data1D &data);
    // Compute integral of supplied data via trapezoid rule between the specified limits
    static double trapezoid(const Data1D &data, double xMin, double xMax);
    // Compute integral of supplied data via trapezoid rule within the specified range
    static double trapezoid(const Data1D &data, const Range range);
    // Compute absolute integral of supplied data via trapezoid rule
    static double absTrapezoid(const Data1D &data);
    // Return sum of all values in supplied data
    static double sum(const Data1D &data);
    // Return sum of supplied data between the specified limits
    static double sum(const Data1D &data, double xMin, double xMax);
    // Return sum of supplied data within the specified range
    static double sum(const Data1D &data, const Range range);
    // Return sum of all absolute values of supplied data
    static double absSum(const Data1D &data);
    // Return sum of all absolute values between the specified limits
    static double absSum(const Data1D &data, double xMin, double xMax);
    // Return sum of all absolute values within the specified range
    static double absSum(const Data1D &data, const Range range);
    // Return sum of squares of all values in supplied data
    static double sumOfSquares(const Data1D &data);
    // Return sum of squares of values between the specified limits
    static double sumOfSquares(const Data1D &data, double xMin, double xMax);
    // Return sum of squares of values within the specified range
    static double sumOfSquares(const Data1D &data, const Range range);
    // Return sum of all values in supplied data
    static double sum(const Data2D &data);
    // Return sum of all absolute values in supplied data
    static double absSum(const Data2D &data);
    // Return sum of all values in supplied data
    static double sum(const Data3D &data);
    // Return sum of all absolute values in supplied data
    static double absSum(const Data3D &data);
};
