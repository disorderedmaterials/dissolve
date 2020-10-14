// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "math/mathfunc.h"
#include "math/constants.h"
#include <cstdlib>
#include <math.h>

/*
 * Error Functions
 */

// Error Function
double DissolveMath::erfc(double x)
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
double DissolveMath::erf(double x) { return (1.0 - erfc(x)); }

/*
 * Random Number Generation
 */

// Random Number Generator (0 - 1)
double DissolveMath::random()
{
    // Simple random number generator from C++ stdlib.
    // Returns numbers from 0.0 to 1.0 inclusive.
    // TODO Better generator
    return (double(rand()) / RAND_MAX);
}

// Return random value between -1 and 1.0 inclusive
double DissolveMath::randomPlusMinusOne() { return (random() - 0.5) * 2.0; }

// Random number generator (0 - RAND_MAX)
int DissolveMath::randomimax()
{
    // Returns a random number from 0->(RAND_MAX-1) inclusive.
    return rand();
}

// Random number generator (0 - range-1)
int DissolveMath::randomi(int range)
{
    // Returns a random number from 0->(range-1) inclusive.
    return int(range * (double(rand() - 1) / RAND_MAX));
}

/*
 * Integer Functions
 */

// Integer power function
int DissolveMath::power(int i, int p)
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
int DissolveMath::sgn(int x) { return (x < 0 ? -1 : (x == 0 ? 0 : 1)); }

// Sign function
int DissolveMath::sgn(double x) { return (x < 0.0 ? -1 : x > 0.0); }

// Apply sign of second argument to first
double DissolveMath::sgn(double a, double signOf) { return signOf >= 0.0 ? fabs(a) : -fabs(a); }

// Return the cyclic permutation of the integer 'i', span 3
int DissolveMath::cp3(int i) { return (i % 3); }
