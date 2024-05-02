// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once
#include <oneapi/dpl/iterator>
#include <oneapi/tbb/blocked_range2d.h>
#include <oneapi/tbb/combinable.h>
#include <oneapi/tbb/parallel_reduce.h>

namespace dissolve
{

template <typename T> using counting_iterator = dpl::counting_iterator<T>;

template <typename T> using combinable = tbb::combinable<T>;

template <typename RowType, typename ColType> using blocked_range2d = tbb::blocked_range2d<RowType, ColType>;

template <typename Range, typename T, typename Op, typename ReductionOp>
T tbb_parallel_reduce(Range &&range, T init, Op &&op, ReductionOp &&reductionop)
{
    return tbb::parallel_reduce(std::forward<Range>(range), init, std::forward<Op>(op), std::forward<ReductionOp>(reductionop));
}

} // namespace dissolve
