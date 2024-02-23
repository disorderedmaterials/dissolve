// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"
#include <cmath>

std::pair<int, int> Combinations::nthCombination(int n) const
{
    auto [a2, remainder] = getCoefficent(n);
    return {a2, remainder};
}

int Combinations::nChooseK(int n, int k) const
{
    // Function n choose k specialised for either k = 1 or k = 2
    switch (k)
    {
        case 1:
            return n;
        case 2:
            return (n * (n - 1)) >> 1;
        default:
            return -1;
    }
}

Combinations::NthCombination Combinations::getCoefficent(int N) const
{
    double a = -0.5;
    double b = N_ - 0.5;

    int i = std::floor((-b + std::sqrt(b * b + 4 * a * N)) / (2 * a));
    int j = N - std::floor(a * i * i + b * i) + i + 1;
    return {i, j};
}

int Combinations::getNumCombinations() const { return nChooseK(N_, k_); }
