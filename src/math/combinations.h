// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once
#include <utility>

// This class implements a combinatorial number system
// https://en.wikipedia.org/wiki/Combinatorial_number_system
// to map the set {0, 1, ...} to the natural number N
// e.g the natural numbers 0....N map to:
// N = 0 -> (0, 1)
// N = 1 -> (0, 2)
// This can be used to loop over each combination in a list.
// For example, if the list is {'A','B', 'C', 'D', 'E'}, the combination pairs are:
// AB AC AD AE BC BD BE CD CE DE which can be found from:

// Combinations comb(5, 2)
// std::vector<char> list = {'A', 'B', 'C', 'D', 'E'}
// for (int i = 0 ; i < comb.getNumCombinations() ; ++i){
//      auto [m, n] = nthCombination(i);
//      auto c1 = list[m];
//      auto c2 = list[n];
//}
class Combinations
{
    struct NthCombination
    {
        int a;
        int remainder;
    };

    public:
    Combinations(int N, int k) : N_(N), k_(k) {}
    // Returns the nth combination
    std::pair<int, int> nthCombination(int n) const;
    // returns the number of combinations
    int getNumCombinations() const;

    private:
    // returns n choose k (specialised for k = 1 and k = 2)
    int nChooseK(int N, int k) const;
    // returns the bionomial coefficent,
    // see https : // en.wikipedia.org/wiki/Combinatorial_number_system
    NthCombination getCoefficent(int n, int k) const;

    int N_;
    int k_;
};