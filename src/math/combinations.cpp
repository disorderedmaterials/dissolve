// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"
#include <cmath>

std::pair<int, int> Combinations::nthCombination(int n) const
{
<<<<<<< HEAD
    /*
      If we let N_ be the number of items being combined and n be the
      index of the pair {x, y}.  The smallest value of n for and value
      of i is given by the relation:
=======
  /*
    If we let x be the 
   */
    auto a = -0.5;
    auto b = N_ - 0.5;
>>>>>>> origin/1806_faster_combinations

      -x² + (2N_ -1)x = 2n

      By the quadratic formula, we can solve for i for a given n.  The
      floor of that value is the x value for our pair and, by plugging
      it ack into the relation, we can find the offset for the y value
      as well.
     */
    int a = -1;
    int b = 2 * N_ - 1;

    int x = std::floor((-b + std::sqrt(b * b + 4 * a * 2 * n)) / (2 * a));
    int y = n - (a * x * x + b * x) / 2 + x + 1;
    return {x, y};
}

int Combinations::getNumCombinations() const { return N_ * (N_ - 1) / 2; }
