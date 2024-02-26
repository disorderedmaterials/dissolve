// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/polynomial.h"
#include "templates/algorithms.h"

#include <gtest/gtest.h>
#include <random>

namespace UnitTest
{

TEST(QuadraticTest, SimpleTest)
{

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(-100.0, 100.0);
    for (auto i = 0; i < 100; ++i)
    {
        auto r1 = dis(gen), r2 = dis(gen);
        auto a = 1;
        auto b = -r1 - r2;
        auto c = r1 * r2;

        if (r2 < r1)
            std::swap(r1, r2);

        Quadratic poly(a, b, c);
        ASSERT_TRUE(poly.hasRoots());
        auto roots = poly.roots();
        EXPECT_NEAR(r1, roots.first, 1e-9);
        EXPECT_NEAR(r2, roots.second, 1e-9);
    }
}
} // namespace UnitTest
