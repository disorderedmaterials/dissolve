// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/interpolator.h"
#include "math/data1D.h"
#include "templates/algorithms.h"
#include <gtest/gtest.h>

namespace UnitTest
{

constexpr double fineDeltaX = 0.1;

class InterpolatorTest : public ::testing::Test
{
    public:
    InterpolatorTest()
    {
        // Regularly-spaced test data
        regularData_.addPoint(0.0, 0.0);
        regularData_.addPoint(1.0, 3.0);
        regularData_.addPoint(2.0, 1.0);
        regularData_.addPoint(3.0, 0.0);
        regularData_.addPoint(4.0, 4.0);
        regularData_.addPoint(5.0, 10.0);
        regularData_.addPoint(6.0, 7.0);

        // Finer y values at fineDeltaX spacing, starting from 0.0, from previous confirmed interpolation
        regularFineY_ = {0,
                         0.43238076923076924,
                         0.8567384615384616,
                         1.2650500000000002,
                         1.6492923076923076,
                         2.0014423076923076,
                         2.313476923076923,
                         2.5773730769230765,
                         2.785107692307692,
                         2.928657692307692,
                         2.999999999999999,
                         2.994134615384615,
                         2.9181538461538463,
                         2.782173076923077,
                         2.596307692307692,
                         2.370673076923076,
                         2.1153846153846145,
                         1.840557692307691,
                         1.556307692307691,
                         1.2727499999999983,
                         0.9999999999999988,
                         0.7470807692307679,
                         0.5186461538461525,
                         0.3182576923076911,
                         0.1494769230769219,
                         0.015865384615383574,
                         -0.07901538461538526,
                         -0.13160384615384638,
                         -0.13833846153846097,
                         -0.09565769230769128,
                         1.6431300764452367e-15,
                         0.15154230769231022,
                         0.359261538461542,
                         0.6227961538461585,
                         0.9417846153846213,
                         1.3158653846153916,
                         1.7446769230769315,
                         2.2278576923077025,
                         2.7650461538461655,
                         3.355880769230782,
                         4.0000000000000115,
                         4.69375000000001,
                         5.4203076923077,
                         6.159557692307697,
                         6.891384615384618,
                         7.595673076923077,
                         8.252307692307689,
                         8.841173076923074,
                         9.342153846153842,
                         9.735134615384611,
                         9.999999999999996,
                         10.12245769230769,
                         10.111507692307695,
                         9.981973076923083,
                         9.74867692307693,
                         9.426442307692321,
                         9.030092307692325,
                         8.57445000000002,
                         8.074338461538485,
                         7.544580769230796,
                         7.000000000000028};
    }

    protected:
    Data1D regularData_;
    std::vector<double> regularFineY_;
};

TEST_F(InterpolatorTest, RegularBasic)
{
    // Form interpolation of data and check interpolation at known values
    Interpolator I(regularData_);
    for (auto &&[x, y] : zip(regularData_.xAxis(), regularData_.values()))
        EXPECT_DOUBLE_EQ(y, I.y(x));

    auto x = 0.0;
    for (auto y : regularFineY_)
    {
        EXPECT_DOUBLE_EQ(y, I.y(x));
        x += fineDeltaX;
    }
}

TEST_F(InterpolatorTest, RegularSequentialTest)
{
    // Form interpolation of regular data
    Interpolator I(regularData_);

    // Create a dummy data1D with fine spacing that we will add the interpolated data in to
    Data1D fineBins;
    auto x = 0.0;
    for (auto n = 0; n < regularFineY_.size(); ++n)
    {
        fineBins.addPoint(x, 0.0);
        x += fineDeltaX;
    }
    Interpolator::addInterpolated(regularData_, fineBins);

    // Check against pre-calculated y values not present in the source data
    for (auto &&[x, y, interpy] : zip(fineBins.xAxis(), regularFineY_, fineBins.values()))
    {
        EXPECT_DOUBLE_EQ(y, I.y(x));
        EXPECT_DOUBLE_EQ(y, interpy);
    }
}

TEST_F(InterpolatorTest, RegularRandomTest)
{
    // Form interpolation of regular data
    Interpolator I(regularData_);

    // Check values in a non-sequential way
    auto indices = {0, 6, 3, 40, 3, 20, 50, 41, 12};
    for (auto i : indices)
        EXPECT_NEAR(regularFineY_[i], I.y(fineDeltaX * i), 1.0e-10);
}

} // namespace UnitTest
