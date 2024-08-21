// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <vector>

// Data Limits
namespace Limits
{
    // Return non-zero values present in whole dataset
    template <typename X, typename Y> std::vector<std::tuple<X, Y>> nonZeroValues(std::vector<X> x, std::vector<Y> values);

    // Return index and corresponding maximum value over all data points
    template <typename T> std::pair<size_t, T> maxValueAt(std::vector<T> values);
}