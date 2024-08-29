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

/*
 * Logaritmic Functions
*/
// Natural (base-e) logarithm of n
int log(int n);
// Natural (base-e) logarithm of x
double log(double x);
// Base-2 logarithm of n
int log2(int n);
// Base-2 logarithm of x
double log2(double x);
// Base-10 logarithm of n
int log10(int n);
// Base-2 logarithm of x
double log10(double x);

}; // namespace DissolveMath
