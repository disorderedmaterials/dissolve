// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/combinations.h"

std::pair<int, int> Combinations::nthIndexPair(int n) const
{
    auto [a2, remainder] = getCoefficent(n, 2);
    auto [a1, remainder_a1] = getCoefficent(remainder, 1);
    return {a1, a2};
}

int Combinations::nChooseK(int n, int k) const
{
    if (k > n)
        return 0;
    if (k * 2 > n)
        k = n - k;
    if (k == 0)
        return 1;

    int result = n;
    for (int i = 2; i <= k; ++i)
    {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}

Combinations::NthCombination Combinations::getCoefficent(int N, int k) const
{

    int at = k - 1;
    int nCt = 0;
    if (nCt >= N)
    {
        return {at, N - nCt};
    }

    int atPrev = at;
    int nCtPrev = nCt;
    bool found = false;
    while (!found)
    {
        atPrev = at;
        at = atPrev + 1;
        nCtPrev = nCt;
        nCt = nChooseK(at, k);
        if (nCt > N)
        {
            found = true;
        }
    }
    return {atPrev, N - nCtPrev};
}

int Combinations::getNumCombinations() const { return nChooseK(N_, k_); }
