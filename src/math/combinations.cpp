// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"
#include <cmath>

std::pair<int, int> Combinations::nthCombination(int n) const
{
  /*
    If we let x be the 
   */
    auto a = -0.5;
    auto b = N_ - 0.5;

    int i = std::floor((-b + std::sqrt(b * b + 4 * a * n)) / (2 * a));
    int j = n - std::floor(a * i * i + b * i) + i + 1;
    return {i, j};
}

int Combinations::getNumCombinations() const { return N_ * (N_-1) / 2; }
