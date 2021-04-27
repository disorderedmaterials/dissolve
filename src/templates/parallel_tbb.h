// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include <tbb/combinable.h>
#include <tbb/parallel_for.h>

namespace algorithms::paralleltbb
{

// returns a tbb iteration range between begin and end
inline auto blocked_range(int begin, int end) { return tbb::blocked_range<int>(begin, end); }

// creates a tbb combinable type which acts as thread local storage for the tbb parallel algorithms
template <typename Type, typename Lambda> inline auto combinable(Lambda &&lambda)
{
    return tbb::combinable<Type>(std::forward<Lambda>(lambda));
}

// runs the tbb parallel_for algorithm
template <typename... Args> void parallel_for(Args &&... args) { tbb::parallel_for(std::forward<Args>(args)...); }

// Parallel reduction using tbb::combinable as thread local storage
// returns the combination (by addition) of the thread local storage instances
template <typename CombinableType, typename... Args>
auto parallel_for_reduction(tbb::combinable<CombinableType> &combinable, Args &&... args) -> CombinableType
{
    parallel_for(std::forward<Args>(args)...);
    return combinable.combine(std::plus<CombinableType>());
}

} // namespace algorithms::paralleltbb