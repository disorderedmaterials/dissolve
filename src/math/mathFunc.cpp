// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/mathFunc.h"
#include "math/constants.h"
#include "math/pcg.h"
#include <cmath>
#include <cstdlib>
#include <random>

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

// Main RNG object
pcg32 rng_(pcg_extras::seed_seq_from<std::random_device>{});

// Set random number generator seed
void setRandomSeed(int seed) { rng_.seed(seed); }

// Return a random number between 0.0 and 1.0 inclusive
double random() { return double(rng_()) / (pcg32::max() - 1); }

// Return a random number between -1.0 and 1.0 inclusive
double randomPlusMinusOne() { return (random() - 0.5) * 2.0; }

// Return a random integer between 0 and N-1 inclusive
int randomi(int N) { return rng_(N); }

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

} // namespace DissolveMath
