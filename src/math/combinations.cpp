// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"
#include "math/polynomial.h"
#include <cmath>
#include <iostream>

std::pair<int, int> Combinations::nthCombination(int n) const
{
    /*
      If we let N_ be the number of items being combined and n be the
      index of the pair {x, y}.  The smallest value of n for and value
      of i is given by the relation:

      -x² + (2N_ -1)x = 2n

      By the quadratic formula, we can solve for i for a given n.  The
      floor of that value is the x value for our pair and, by plugging
      it ack into the relation, we can find the offset for the y value
      as well.
     */
    Quadratic relation(-1, 2*N_ - 1, -2 * n);

    auto roots = relation.roots();
    auto x = std::floor(relation.roots().second);
    auto y = n - (relation.at(x) + 2 * n)/ 2 + x + 1;

    std::cout << roots.first << ", " << roots.second << " ( " << x << ", " << y << ") " << relation.at(x) << std::endl;

    return {x, y};
}

int Combinations::getNumCombinations() const { return N_ * (N_ - 1) / 2; }
