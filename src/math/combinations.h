// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <utility>

class Combinations
{
    struct NthCombination
    {
        int a;
        int remainder;
    };

    public:
    Combinations(int N, int k) : N_(N), k_(k) {}
    std::pair<int, int> nthIndexPair(int n) const;
    int getNumCombinations() const;

    private:
    int nChooseK(int N, int k) const;
    NthCombination getCoefficent(int n, int k) const;

    int N_;
    int k_;
};