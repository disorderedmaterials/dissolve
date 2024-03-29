// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once
struct FakeParallelPolicy
{
};
#ifdef MULTITHREADING
#include "tbbDefs.h"
#if defined(__clang__)
#include <oneapi/dpl/algorithm>
#include <oneapi/dpl/execution>
#include <oneapi/dpl/numeric>
namespace ParallelPolicies
{
constexpr auto par = dpl::execution::par;
constexpr auto seq = dpl::execution::seq;
constexpr auto par_unseq = dpl::execution::par_unseq;
} // namespace ParallelPolicies
#else
#include <algorithm>
#include <execution>
#include <numeric>
namespace ParallelPolicies
{
constexpr auto par = std::execution::par;
constexpr auto seq = std::execution::seq;
constexpr auto par_unseq = std::execution::par_unseq;
} // namespace ParallelPolicies
#endif // end #if defined(__clang__)
namespace dissolve::internal
{
template <typename T> using is_execution_policy = std::is_execution_policy<T>;
}
#else // not multithreading - no tbb libraries
#include "tbbFallbacks.h"
#include <algorithm>
#include <numeric>
namespace dissolve::internal
{
// create a dummy 'is_execution_policy' that always returns false
template <typename T> struct is_execution_policy : std::false_type
{
};
} // namespace dissolve::internal
namespace ParallelPolicies
{
constexpr FakeParallelPolicy par{};
constexpr FakeParallelPolicy seq{};
constexpr FakeParallelPolicy par_unseq{};
} // namespace ParallelPolicies
#endif // end #ifdef MULTITHREADING
