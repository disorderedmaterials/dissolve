// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/mathFunc.h"
#include "math/constants.h"
#include <cmath>
#include <cstdlib>

namespace DissolveMath
{

/*
 * Error Functions
 */

// Error Function
double erfc(double x)
{
    // Approximation to the complementary error function.
    // Ref: Abramowitz and Stegun, Handbook of Mathematical Functions,
    //	National Bureau of Standards, Formula 7.1.26
    static double a1 = 0.254829592, a2 = -0.284496736, a3 = 1.421413741, a4 = -1.453152027, a5 = 1.061405429, p = 0.3275911;
    double t, tp, result;
    t = 1.0 / (1.0 + p * x);
    tp = t * (a1 + t * (a2 + t * (a3 + t * (a4 + t * a5))));
    result = tp * exp(-(x * x));
    return result;
}

// Complementary error function
double erf(double x) { return (1.0 - erfc(x)); }

/*
 * Random Number Generation
 */

// Random Number Generator (0 - 1)
double random()
{
    // Simple random number generator from C++ stdlib.
    // Returns numbers from 0.0 to 1.0 inclusive.
    // TODO Better generator
    return (double(rand()) / RAND_MAX);
}

// Return random value between -1 and 1.0 inclusive
double randomPlusMinusOne() { return (random() - 0.5) * 2.0; }

// Random number generator (0 - RAND_MAX)
int randomimax()
{
    // Returns a random number from 0->(RAND_MAX-1) inclusive.
    return rand();
}

// Random number generator (0 - range-1)
int randomi(int range)
{
    // Returns a random number from 0->(range-1) inclusive.
    return int(range * (double(rand() - 1) / RAND_MAX));
}

/*
 * Integer Functions
 */

// Integer power function
int power(int i, int p)
{
    static int result, n;
    result = i;
    if (p == 0)
        result = 1;
    else
        for (n = 1; n < p; n++)
            result *= i;
    return result;
}

// Sign function
int sgn(int x) { return (x < 0 ? -1 : (x == 0 ? 0 : 1)); }

// Sign function
int sgn(double x) { return (x < 0.0 ? -1 : x > 0.0); }

// Apply sign of second argument to first
double sgn(double a, double signOf) { return signOf >= 0.0 ? fabs(a) : -fabs(a); }

// Return the cyclic permutation of the integer 'i', span 3
int cp3(int i) { return (i % 3); }

/*
 * Logaritmic Functions
*/
// Natural (base-e) logarithm of n
int log(int n) { return int(std::log(n)); }
// Natural (base-e) logarithm of x
double log(double x) { return std::log(x); }
// Base-2 logarithm of n
int log2(int n) { return int(std::log2(n)); }
// Base-2 logarithm of x
double log2(double x) { return std::log2(x); }
// Base-10 logarithm of n
int log10(int n) { return int(std::log10(n)); }
// Base-2 logarithm of x
double log10(double x) { return std::log10(x); }

} // namespace DissolveMath