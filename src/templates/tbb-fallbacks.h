// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once
#include <cassert>
#include <numeric>
#include <vector>

namespace dissolve
{
// A less optimized version of the counting iterator if tbb is not available
template <typename T> class counting_iterator
{
    public:
    counting_iterator(int N, int M)
    {
        assert(M > N);
        indices_.resize(M - N, T{});
        std::iota(indices_.begin(), indices_.end(), N);
    }
    auto begin() { return indices_.begin(); }
    auto end() { return indices_.end(); }

    private:
    std::vector<T> indices_;
};

} // namespace dissolve