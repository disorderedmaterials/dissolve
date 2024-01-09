// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class A> class Array2D;

// Mathematical functions
namespace DissolveMath
{
/*
 * Error Functions
 */
double erfc(double);
double erf(double);

/*
 * Random Number Generation
 */
double random();
double randomPlusMinusOne();
int randomimax();
int randomi(int range);

/*
 * Integer Functions
 */
// Raise the integer 'i' to the power 'p'
int power(int i, int p);
// Sign function
int sgn(int x);
// Sign function
int sgn(double x);
// Apply sign of second argument to first
double sgn(double a, double signOf);
// Return the cyclic permutation of the integer 'i', span 3
int cp3(int i);
}; // namespace DissolveMath
