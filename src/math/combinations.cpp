// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"

std::pair<int, int> Combinations::nthCombination(int n) const
{
    auto [a2, remainder] = getCoefficent(n, 2);
    auto [a1, remainder_a1] = getCoefficent(remainder, 1);
    return {a1, a2};
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

Combinations::NthCombination Combinations::getCoefficent(int N, int k) const
{

    auto at = k - 1;
    auto nCt = 0;
    if (nCt >= N)
        return {at, N - nCt};

    auto atPrev = at;
    auto nCtPrev = nCt;
    bool found = false;
    while (!found)
    {
        atPrev = at;
        at = atPrev + 1;
        nCtPrev = nCt;
        nCt = nChooseK(at, k);
        if (nCt > N)
            found = true;
    }
    return {atPrev, N - nCtPrev};
}

int Combinations::getNumCombinations() const { return nChooseK(N_, k_); }
