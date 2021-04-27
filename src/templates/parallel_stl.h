// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once
#ifdef MULTITHREADING
#if defined(__clang__)
#include <pstl/algorithm>
#include <pstl/execution>
namespace ParallelPolicies
{
constexpr auto par = pstl::execution::par;
constexpr auto seq = pstl::execution::seq;
constexpr auto par_unseq = pstl::execution::par_unseq;
} // namespace ParallelPolicies
#else
#include <algorithm>
#include <execution>
namespace ParallelPolicies
{
constexpr auto par = std::execution::par;
constexpr auto seq = std::execution::seq;
constexpr auto par_unseq = std::execution::par_unseq;
} // namespace ParallelPolicies
#endif // end #if defined(__clang__)
#endif // end #ifdef MULTITHREADING
