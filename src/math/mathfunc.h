// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class A> class Array2D;

// Mathematical functions
class DissolveMath
{
    /*
     * Error Functions
     */
    public:
    static double erfc(double);
    static double erf(double);

    /*
     * Random Number Generation
     */
    public:
    static double random();
    static double randomPlusMinusOne();
    static int randomimax();
    static int randomi(int range);

    /*
     * Integer Functions
     */
    public:
    // Raise the integer 'i' to the power 'p'
    static int power(int i, int p);
    // Sign function
    static int sgn(int x);
    // Sign function
    static int sgn(double x);
    // Apply sign of second argument to first
    static double sgn(double a, double signOf);
    // Return the cyclic permutation of the integer 'i', span 3
    static int cp3(int i);
};
