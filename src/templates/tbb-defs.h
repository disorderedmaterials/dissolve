// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once
#include "tbb/iterators.h"
namespace dissolve
{
template <typename T> class counting_iterator
{
    public:
    counting_iterator(int N, int M) : N_(N), M_(M) { assert(M > N); }
    auto begin() { return tbb::counting_iterator<int>(N_); }
    auto end() { return tbb::counting_iterator<int>(M_); }

    private:
    int N_;
    int M_;
};

} // namespace dissolve