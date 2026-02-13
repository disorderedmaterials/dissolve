// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/regression.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(RegressionTest, LinearTriangle)
{
    Data1D data;
    // "Triangular" data, equally spaced
    data.addPoint(0.0, 0.0);
    data.addPoint(1.0, 1.0);
    data.addPoint(2.0, 2.0);
    data.addPoint(3.0, 3.0);
    data.addPoint(4.0, 2.0);
    data.addPoint(5.0, 1.0);
    data.addPoint(6.0, 0.0);

    auto yBar = 0.0;
    auto gradient = Regression::linearGradient(data, 7, yBar);
    EXPECT_NEAR(gradient, 0.0, 1.0e-6);
    EXPECT_DOUBLE_EQ(yBar, 9.0 / 7.0);

    // Equally spaced, so regression on just the y values should yield same answer
    gradient = Regression::linearGradient(data.values(), 7, yBar);
    EXPECT_NEAR(gradient, 0.0, 1.0e-6);
    EXPECT_DOUBLE_EQ(yBar, 9.0 / 7.0);
}

TEST(RegressionTest, LinearRandom)
{
    Data1D data;
    data.addPoint(0.0, 1.4);
    data.addPoint(0.3, 3.8);
    data.addPoint(0.9, 2.9);
    data.addPoint(1.5, 2.6);
    data.addPoint(1.7, 2.6);
    data.addPoint(2.5, 4.8);
    data.addPoint(4.0, 3.5);

    auto yBar = 0.0;
    auto gradient = Regression::linearGradient(data, 7, yBar);
    EXPECT_NEAR(gradient, 0.3919, 1.0e-4);
    EXPECT_DOUBLE_EQ(yBar, (1.4 + 3.8 + 2.9 + 2.6 + 2.6 + 4.8 + 3.5) / 7.0);
}

} // namespace UnitTest
