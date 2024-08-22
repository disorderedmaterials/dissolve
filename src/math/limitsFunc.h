// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/algorithms.h"
#include <algorithm>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>

// Data Limits
namespace DissolveLimits
{
// Return non-zero values present in whole dataset
template <typename X, typename Y> std::vector<std::tuple<X, Y>> nonZeroValues(std::vector<X> x, std::vector<Y> values)
{
    std::vector<std::tuple<X, Y>> pairs;

    auto xys = zip(x, values);

    std::copy_if(xys.begin(), xys.end(), std::back_inserter(pairs), [](const auto &val) { return std::get<1>(val) > 0; });

    return pairs;
}

// Return index and corresponding maximum value over all data points
template <typename T> std::pair<std::size_t, T> maxValueAt(std::vector<T> values)
{
    if (values.empty())
        return {0, 0.0};

    std::vector<double>::iterator result = std::max_element(values.begin(), values.end());

    // { index, maxvalue }
    return {std::distance(values.begin(), result), *result};
}

}; // namespace DissolveLimits
