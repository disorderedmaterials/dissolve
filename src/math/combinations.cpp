// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"
#include "math/polynomial.h"
#include <cmath>

std::pair<int, int> Combinations::nthCombination(int n) const
{
    /*
      If we let N_ be the number of items being combined and n be the
      index of the pair {x, y}.  The smallest value of n for and value
      of i is given by the relation:

      -½x² + (N_ -½1)x = n

      By the quadratic formula, we can solve for i for a given n.  The
      floor of that value is the x value for our pair and, by plugging
      it ack into the relation, we can find the offset for the y value
      as well.
     */
    Quadratic relation(-0.5, N_ - 0.5, -n);

    auto roots = relation.roots();
    auto x = std::floor(relation.roots().second);
    auto y = x + 1 - relation.at(x);

    return {x, y};
}

int Combinations::getNumCombinations() const { return N_ * (N_ - 1) / 2; }
