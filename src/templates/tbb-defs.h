// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once
#include "tbb/combinable.h"
#include "tbb/iterators.h"
namespace dissolve
{

template <typename T> using counting_iterator = tbb::counting_iterator<T>;

template <typename T> using combinable = tbb::combinable<T>;

} // namespace dissolve